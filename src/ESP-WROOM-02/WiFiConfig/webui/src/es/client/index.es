// vim:ft=javascript
export default class Index {
	constructor(window_obj){
		this.w=window_obj
		this.d=this.w.document
		this.events=[];
		this.ap=[];
	}
	id(target) {
		return this.d.getElementById(target)
	}
	click(id,li) {
		const el=this.id(id);
		el.addEventListener('click',li);
		this.events.push([el,'click',li]);
	}
	clear_events() {
		for(let i in this.events) {
			const e=this.events[i];
			e[0].removeEventListener(e[1],e[2])
		}
		this.events=[]
	}
	show(id) {
		this.id(id).style.display = 'block';
	}
	hide(id) {
		this.id(id).style.display = 'none';
	}
	fetch_json(url,data){
		return new Promise((resolve,reject)=>{
			const request = new XMLHttpRequest();
			if(data) {
				console.log("Fetch(POST): ",url)
				request.open('POST', url, true);
				request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');
			} else {
				console.log("Fetch(GET): ",url)
				request.open('GET', url, true);
			}
			request.onload=function(){
				if (this.status >= 200 && this.status < 400) {
					resolve(JSON.parse(this.response))
				} else {
					console.log(this)
					reject()
				}
			}
			if(data) {
				let params=[]
	    		for( let name in data ) {
					const value = data[ name ];
					const param = encodeURIComponent( name ) + '=' + encodeURIComponent( value );
					params.push( param );
				}
				const d=params.join( '&' ).replace( /%20/g, '+' );
				console.log(d)
				request.send(d)
			} else {
				request.send()
			}
		})
	}
	run() {
		console.log("Index.run");
		this.fetch_ap_list();

	}
	reset_disp_ap() {
		this.hide('d_ap_list');
		this.hide('d_ap_scan_now');
		this.hide('d_ap_scan');
		this.hide('d_ap_info');
		this.show('d_ap');
		this.clear_events();
	}

	fetch_ap_list() {
		this.reset_disp_ap();
		this.fetch_json('/api/ap/list').then((d)=>{
			let html='<tr><td>番号</td><td>SSID</td><td>　</td></tr>';
			for(let i in d.data) {
				this.ap[i]={ ssid: d.data[i].ssid, passphase: '', update: false }
			}
			this.show_ap_list()
		})
	}

	show_ap_list() {
		this.reset_disp_ap();
		let html='<tr><td>番号</td><td>SSID</td><td>　</td></tr>';

		let update=false;
		for(let i in this.ap) {
			const v=this.ap[i]
			html=html+'<tr>'
				+'<td>'+(new Number(i)+1)+'</td>'
				+ ( v.update ? '<td style="background-color:#FFFFAA">' : '<td>' ) +v.ssid+'</td>'
				+'<td><input type="button" id="b_ap_setup_'+i+'" value="設定"></td>';
				+'</tr>';
			if(v.update) { update=true }
		}
		this.id('t_ap_list').innerHTML=html;
		for(let i in this.ap) { ((j)=>{ this.click('b_ap_setup_'+j,(e)=>{ this.show_ap_info(j) }) })(i); }

		this.click('b_submit',()=>{ this.do_submit() })

		this.click('b_clear',()=>{
			this.hide('d_submit')
			this.hide('d_submit_warn')
			this.fetch_ap_list()
		})

		this.click('b_reset',()=>{
			if(this.w.confirm("工場出荷時に戻します。よろしいですか？")) { this.do_reset() }
		})

		if(update) {
			this.show('d_submit')
			this.show('d_submit_warn')
		}
		this.show('d_ap_list')
	}

	do_submit() {
		this.reset_disp_ap();
		this.hide('d_submit_warn')
		this.show('d_submit_exec')
		let form={};
		for(let i in this.ap) {
			form["ssid"+i]=this.ap[i].ssid
			form["passphase"+i]=this.ap[i].passphase
			form["update"+i]=this.ap[i].update ? 1 : 0
		}
		this.fetch_json('/api/ap/config',form)
	}

	do_reset() {
		this.reset_disp_ap();
		this.hide('d_submit_warn')
		this.show('d_reset_exec')
		this.fetch_json('/api/ap/reset',{ execute_reset: 42 } )
	}

	show_ap_info(n) {
		this.reset_disp_ap();
		this.id('s_ap_info_num').innerHTML=(new Number(n)+1);

		this.id('t_ap_info_ssid').value=this.ap[n].ssid
		this.id('t_ap_info_passphase').value=this.ap[n].passphase

		this.click('b_ap_info_back',  (e)=>{ this.show_ap_list() })
		this.click('b_ap_info_scan',  (e)=>{ this.fetch_ap_scan(n) })
		this.click('b_ap_info_commit',(e)=>{
			const ssid=this.id('t_ap_info_ssid').value
			const passphase=this.id('t_ap_info_passphase').value
			this.ap[n]={ ssid: ssid, passphase: passphase, update: true }
			this.show_ap_list()
		})
		this.show('d_ap_info')
	}

	fetch_ap_scan(n) {
		this.reset_disp_ap();
		this.id('s_ap_scan_num').innerHTML=(new Number(n)+1);
		this.show('d_ap_scan_now');

		this.fetch_json('/api/ap/scan').then((d)=>{
			let html='<tr><td>SSID</td><td>Ch.</td><td>RSSI</td><td></td></tr>';
			d.data.sort((a,b)=>{ return b.rssi - a.rssi })
			for(let i in d.data) {
				const v=d.data[i]
				html=html+'<tr><td>'
					+v.ssid+'</td><td>'
					+v.channel+'</td><td>'
					+v.rssi+'dB</td>'
					+'<td><input type="button" id="t_ap_scan_sel_'+i+'" value="選択"></tr>';
			}
			this.id('t_ap_scan').innerHTML=html;
			for(let i in d.data) {
				((j)=>{
					this.click('t_ap_scan_sel_'+j,(e)=>{
						this.ap[n].ssid=d.data[j].ssid
						this.ap[n].passphase=""
						this.show_ap_info(n)
					})
				})(i)
			}

			this.click('b_ap_scan_back', (e)=>{ this.show_ap_info(n) })
			this.hide('d_ap_scan_now');
			this.show('d_ap_scan')
		})
	}

}

