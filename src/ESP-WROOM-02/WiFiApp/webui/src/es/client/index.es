// vim:ft=javascript

import Utils from './utils.es'

export default class Index {
	constructor(w){
		this.u=new Utils(w)
		this.ap=[];
	}
	run() {
		console.log("Index");
		this.ap_list_load();

	}
	reset_disp_ap() {
		this.u
			.hide('d_ap_list')
			.hide('d_ap_scan_now')
			.hide('d_ap_scan')
			.hide('d_ap_info')
			.show('d_ap')
			.show('m_wifi')
			.clear_events()
	}
	ap_list_load() {
		this.reset_disp_ap();
		this.u.json('/api/ap/list').then((d)=>{
			let html='<tr><td>番号</td><td>SSID</td><td>　</td></tr>';
			for(let i in d.data) {
				this.ap[i]={ ssid: d.data[i].ssid, passphase: '', update: false }
			}
			this.ap_list_show()
		})
	}
	ap_list_show() {
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
		this.u.id('t_ap_list').innerHTML=html;

		for(let i in this.ap) {((j)=>{
			this.u.click('b_ap_setup_'+j,(e)=>{ this.ap_info_edit(j) })
		})(i)}

		this.u
			.click('b_submit',()=>{ this.do_submit() })
			.click('b_clear',()=>{
				this.u
					.hide('d_submit')
					.hide('d_submit_warn')
				this.ap_list_load()
			})
			.click('b_reset',()=>{
				if(this.u.w.confirm("工場出荷時に戻します。よろしいですか？")) { this.do_reset() }
			})

		if(update) {
			this.u
				.show('d_submit')
				.show('d_submit_warn')
		}
		this.u.show('d_ap_list')
	}
	do_submit() {
		this.reset_disp_ap();
		this.u
			.hide('d_submit_warn')
			.show('d_submit_exec')
		let form={};
		for(let i in this.ap) {
			form["ssid"+i]=this.ap[i].ssid
			form["passphase"+i]=this.ap[i].passphase
			form["update"+i]=this.ap[i].update ? 1 : 0
		}
		this.u.json('/api/ap/config',form)
	}
	do_reset() {
		this.reset_disp_ap();
		this.u
			.hide('d_submit_warn')
			.show('d_reset_exec')
			.json('/api/ap/reset',{ execute_reset: 42 } )
	}
	ap_info_edit(n) {
		this.reset_disp_ap();
		this.u.id('s_ap_info_num').innerHTML=(new Number(n)+1);
		this.u.id('t_ap_info_ssid').value=this.ap[n].ssid
		this.u.id('t_ap_info_passphase').value=this.ap[n].passphase

		this.u
			.click('b_ap_info_back',  (e)=>{ this.ap_list_show() })
			.click('b_ap_info_scan',  (e)=>{ this.ap_scan_show(n) })
			.click('b_ap_info_commit',(e)=>{
				const ssid=this.u.id('t_ap_info_ssid').value
				const passphase=this.u.id('t_ap_info_passphase').value
				this.ap[n]={ ssid: ssid, passphase: passphase, update: true }
				this.ap_list_show()
			})
			.show('d_ap_info')
	}
	ap_scan_show(n) {
		this.reset_disp_ap();
		this.u.id('s_ap_scan_num').innerHTML=(new Number(n)+1);
		this.u.show('d_ap_scan_now');
		this.u.json('/api/ap/scan').then((d)=>{
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
			this.u.id('t_ap_scan').innerHTML=html;
			for(let i in d.data) {
				((j)=>{
					this.u.click('t_ap_scan_sel_'+j,(e)=>{
						this.ap[n].ssid=d.data[j].ssid
						this.ap[n].passphase=""
						this.ap_info_edit(n)
					})
				})(i)
			}
			this.u
				.click('b_ap_scan_back', (e)=>{ this.ap_info_edit(n) })
				.hide('d_ap_scan_now')
				.show('d_ap_scan')
		})
	}
}

