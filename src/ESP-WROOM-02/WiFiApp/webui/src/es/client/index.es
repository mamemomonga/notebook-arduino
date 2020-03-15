// vim:ft=javascript

import Utils from './utils.es'

class Events {
	constructor(u){
		this.u=u
		this.events=[]
	}
	click(id,li) {
		const el=this.u.id(id);
		el.addEventListener('click',li);
		this.events.push([el,'click',li]);
		return this
	}
	clear() {
		for(let i in this.events) {
			const e=this.events[i];
			e[0].removeEventListener(e[1],e[2])
		}
		this.events=[]
		return this
	}
}

class Pa {
	constructor(p){
		this.stash=p.stash
		this.reset=()=>{ p.reset(); return this }
		this.u=p.u
		this.ev=new Events(this.u)
	}
	show(n) {
		this.number=n
		this.reset().render().events()
		this.u.show(this.ids[0])
		return this
	}
	hide() {
		for(let i in this.ids) {
			this.u.hide(this.ids[i])
		}
		this.ev.clear()
	}
	render(){ return this }
	events(){ return this }
}

class PaAPList extends Pa {
	constructor(p){
		super(p)
		this.ids=['d_ap_list'];
		this.cb={
			edit:  (n)=>{},
		}
	}
	load() {
		console.log("APList load")
		this.u.json('/api/ap/list')
		.then((d)=>{
			for(let i in d.data) {
				this.stash.ap[i]={ ssid: d.data[i].ssid, passphase: '', update: false }
			}
		}).then(()=>{
			this.cb.committable(false)
			return this.show();
		})
	}
	events() {
		for(let i in this.stash.ap) {((j)=>{
			this.ev.click('b_ap_setup_'+j,(e)=>{ this.cb.edit(j) })
		})(i)}
		return this
	}
	render() {
		let html='<tr><td>番号</td><td>SSID</td><td>　</td></tr>';
		let update=false;
		for(let i in this.stash.ap) {
			const v=this.stash.ap[i]
			html=html+'<tr>'
				+'<td>'+(new Number(i)+1)+'</td>'
				+ ( v.update ? '<td style="background-color:#FFFFAA">' : '<td>' ) +v.ssid+'</td>'
				+'<td><button id="b_ap_setup_'+i+'" class="bt_blue">設定</button></td>';
				+'</tr>';
			if(v.update) { update=true }
		}
		if(update) this.cb.committable(true)
		this.u.id('t_ap_list').innerHTML=html;
		return this;
	}
}

class PaAPInfo extends Pa {
	constructor(p){
		super(p)
		this.ids=['d_ap_info']
		this.cb={
			back:   ( )=>{},
			scan:   (n)=>{},
			apply: ( )=>{},
		}
	}
	render() {
		this.u.id('s_ap_info_num').innerHTML=(new Number(this.number)+1);
		this.u.id('t_ap_info_ssid').value=this.stash.ap[this.number].ssid
		this.u.id('t_ap_info_passphase').value=this.stash.ap[this.number].passphase
		return this
	}
	apply(ssid) {
		this.reset().render().events()
		this.u.id('t_ap_info_ssid').value=ssid
		this.u.show(this.ids[0])
		return this
	}
	events() {
		this.ev.click('b_ap_info_back',()=>{ this.cb.back() })
		this.ev.click('b_ap_info_scan',()=>{ this.cb.scan(this.number) })
		this.ev.click('b_ap_info_commit',()=>{
			const ssid=this.u.id('t_ap_info_ssid').value
			const passphase=this.u.id('t_ap_info_passphase').value
			this.stash.ap[this.number]={ ssid: ssid, passphase: passphase, update: true }
			this.cb.apply()
		})
		return this
	}
}
class PaAPScan extends Pa {
	constructor(p){
		super(p)
		this.ids=['d_ap_scan','d_ap_scan_now']
		this.cb={
			back:  ( )=>{},
			apply: ( )=>{},
		}
		this.d={};
	}
	fetch() {
		return new Promise((resolve,reject)=>{
			this.u.json('/api/ap/scan',{"fetch":1}).then(()=>{
				setTimeout(()=>{
					this.u.json('/api/ap/scan').then((d)=>{
						this.d=d.data;
						resolve()
					}).catch((e)=>{ reject(e) });
				},1000)
			}).catch((e)=>{ reject(e) })
		})
	}
	show(n) {
		this.number=n
		this.u.id('s_ap_scan_num').innerHTML=(new Number(n)+1);
		this.reset()
		this.u.show('d_ap_scan_now');
		this.fetch().then(()=>{
			this.reset().render().events()
			this.u.hide('d_ap_scan_now').show(this.ids[0])
		})
		return this
	}
	render() {
		let html='<tr><td>SSID</td><td>Ch.</td><td>RSSI</td><td></td></tr>';
		this.d.sort((a,b)=>{ return b.rssi - a.rssi })
		for(let i in this.d) {
			const v=this.d[i]
			html=html+'<tr><td>'
				+v.ssid+'</td><td>'
				+v.channel+'</td><td>'
				+v.rssi+'dB</td>'
				+'<td><button id="t_ap_scan_sel_'+i+'" class="bt_blue">選択</button></td>';
				+'</tr>'
		}
		this.u.id('t_ap_scan').innerHTML=html;
		return this
	}
	events() {
		for(let i in this.d) {
			((j)=>{
				this.ev.click('t_ap_scan_sel_'+j,(e)=>{ this.cb.apply(this.d[j].ssid) })
			})(i)
		}
		this.ev.click('b_ap_scan_back', ()=>{ this.cb.back(this.number) })
	}
}
class PaAPCommit extends Pa {
	constructor(p){
		super(p)
		this.ids=['d_ap_commit']
		this.cb={
			cancel: ()=>{},
			apply:  ()=>{},
		}
		this.evb=new Events(this.u);
		this.committable(false)
	}
	committable(n) {
		n ? this.show_buttons() : this.hide_buttons()
	}
	show_buttons() {
		this.evb.click('b_commit',()=>{ this.commit() })
		this.evb.click('b_cancel',()=>{ this.cb.cancel() })
		this.u.show('p_commit')
	}
	hide_buttons() {
		this.evb.clear()
		this.u.hide('p_commit')
	}
	commit() {
		let form={};
		for(let i in this.stash.ap) {
			form["ssid"+i]=this.stash.ap[i].ssid
			form["passphase"+i]=this.stash.ap[i].passphase
			form["update"+i]=this.stash.ap[i].update ? 1 : 0
		}
		this.u.json('/api/ap/config',form)
		this.show()
	}
	show() {
		super.show()
		this.u.hide('p_commit')
		this.cb.apply()
	}
}

class PWiFi {
	constructor(u){
		this.u=u
		this.stash={
			ap: []
		}
		this.ev=new Events(this.u)
	}
	button(m) {
		const n='btc_wifi'
		if(m) {
			this.ev.click(n,()=>{ this.show() })
			this.u.enable(n)
		} else {
			this.u.disable(n)
			this.ev.clear()
		}
	}
	show() {
		const sr={
			u: this.u,
			stash: this.stash,
			reset: ()=>{ this.reset() },
		}
		const pa={
			list:   new PaAPList(sr),
			info:   new PaAPInfo(sr),
			scan:   new PaAPScan(sr),
			commit: new PaAPCommit(sr),
		}
		pa.list.cb={
			edit:  (n)=>{ pa.info.show(n) },
			committable: (n)=>{ pa.commit.committable(n) },
		}
		pa.info.cb={
			back:   ( )=>{ pa.list.show() },
			scan:   (n)=>{ pa.scan.show(n) },
			apply:  ( )=>{ pa.list.show() },
		}
		pa.scan.cb={
			back:  (n   )=>{ pa.info.show(n) },
			apply: (ssid)=>{ pa.info.apply(ssid) },
		}
		pa.commit.cb={
			cancel: ()=>{ pa.list.load() },
			apply:  ()=>{ this.button(true) },
		}
		pa.list.load()
		this.pa=pa

		this.button(false)

		this.reset()
		this.u.show('m_wifi')

		this.data_pane()
		setInterval(()=>{ this.data_pane() },10*1000)
	}
	hide() {
		this.reset()
		this.u.enable('btc_wifi')
		this.u.hide('w_wifi')
	}
	reset() {
		for(let i in this.pa) {
			this.pa[i].hide()
		}
	}
	data_pane() {
		const av=(id,v)=>{
			v=v ? v : '---'
			this.u.id(id).innerHTML=v
		}
		this.u.json('/api/ap/info').then((d)=>{
			av('dp_hostname',d.hostname)
			av('dp_ap_ssid',d.ap.ssid)
			av('dp_ap_ip',d.ap.ip)
			av('dp_sta_ssid',d.sta.ssid)
			av('dp_sta_ip',d.sta.ip)
		})
	}
}

export default class Index {
	constructor(w){
		this.u=new Utils(w)
		this.wifi=new PWiFi(this.u)
		this.ev=new Events(this.u)
	}
	run() {
		this.wifi.button(true)
	}
}

