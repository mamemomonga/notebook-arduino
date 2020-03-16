// vim:ft=javascript
import Utils from './utils.es'
import { Events, PLayer1, PLayer2 } from './pages.es'
const ut=new Utils(window)

class PageWifiList extends PLayer2 {
	constructor(p){
		super(p)
		this.ids=['d_sta_list'];
		this.cb={
			edit:  (n)=>{},
		}
	}
	load() {
		ut.json('/api/ap/list')
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
			this.ev.click('b_sta_setup_'+j,(e)=>{ this.cb.edit(j) })
		})(i)}
		return this
	}
	render() {
		let html='<tr><th>番号</th><th style="min-width:200px">SSID</th><th>　</th></tr>';
		let update=false;
		for(let i in this.stash.ap) {
			const v=this.stash.ap[i]
			html=html+'<tr>'
				+'<td>'+(new Number(i)+1)+'</td>'
				+ ( v.update ? '<td style="background-color:#FFFFAA">' : '<td>' ) +v.ssid+'</td>'
				+'<td><button id="b_sta_setup_'+i+'" class="bt_blue">設定</button></td>';
				+'</tr>';
			if(v.update) { update=true }
		}
		if(update) this.cb.committable(true)
		ut.inner('t_sta_list',html);
		return this;
	}
}

class PageWifiSTAInfo extends PLayer2 {
	constructor(p){
		super(p)
		this.ids=['d_sta_info']
		this.cb={
			back:   ( )=>{},
			scan:   (n)=>{},
			apply: ( )=>{},
		}
	}
	render() {
		ut.inner('s_sta_info_num',(new Number(this.number)+1));
		ut.id('t_sta_info_ssid').value=this.stash.ap[this.number].ssid
		ut.id('t_sta_info_passphase').value=this.stash.ap[this.number].passphase
		return this
	}
	apply(ssid) {
		this.reset().render().events()
		ut.id('t_sta_info_ssid').value=ssid
		ut.show(this.ids[0])
		return this
	}
	events() {
		this.ev.click('b_sta_info_back',()=>{ this.cb.back() })
		this.ev.click('b_sta_info_scan',()=>{ this.cb.scan(this.number) })
		this.ev.click('b_sta_info_commit',()=>{
			const ssid=ut.id('t_sta_info_ssid').value
			const passphase=ut.id('t_sta_info_passphase').value
			this.stash.ap[this.number]={ ssid: ssid, passphase: passphase, update: true }
			this.cb.apply()
		})
		return this
	}
}
class PageWifiSTAScan extends PLayer2 {
	constructor(p){
		super(p)
		this.ids=['d_sta_scan','d_sta_scan_now']
		this.cb={
			back:  ( )=>{},
			apply: ( )=>{},
		}
		this.d={};
	}
	fetch() {
		return new Promise((resolve,reject)=>{
			ut.json('/api/ap/scan',{"fetch":1}).then(()=>{
				setTimeout(()=>{
					ut.json('/api/ap/scan').then((d)=>{
						this.d=d.data;
						resolve()
					}).catch((e)=>{ reject(e) });
				},1000)
			}).catch((e)=>{ reject(e) })
		})
	}
	show(n) {
		this.number=n
		ut.inner('s_sta_scan_num',(new Number(n)+1));
		this.reset()
		ut.show('d_sta_scan_now');
		this.fetch().then(()=>{
			this.reset().render().events()
			ut.hide('d_sta_scan_now').show(this.ids[0])
		})
		return this
	}
	render() {
		let html='<tr><th>SSID</th><th>Ch.</th><th>RSSI</th><th>　</th></tr>';
		this.d.sort((a,b)=>{ return b.rssi - a.rssi })
		for(let i in this.d) {
			const v=this.d[i]
			html=html+'<tr><td>'
				+v.ssid+'</td><td>'
				+v.channel+'</td><td>'
				+v.rssi+'dB</td>'
				+'<td><button id="t_sta_scan_sel_'+i+'" class="bt_blue">選択</button></td>';
				+'</tr>'
		}
		ut.inner('t_sta_scan',html);
		return this
	}
	events() {
		for(let i in this.d) {
			((j)=>{
				this.ev.click('t_sta_scan_sel_'+j,(e)=>{ this.cb.apply(this.d[j].ssid) })
			})(i)
		}
		this.ev.click('b_sta_scan_back', ()=>{ this.cb.back(this.number) })
	}
}

class PageWifiSTACommit extends PLayer2 {
	constructor(p){
		super(p)
		this.ids=['d_sta_commit']
		this.cb={
			apply:  ()=>{},
		}
		this.evb=new Events(ut);
		this.committable(false)
	}
	committable(n) {
		n ? this.show_buttons() : this.hide_buttons()
	}
	show_buttons() {
		this.evb.click('b_commit',()=>{ this.commit() })
		ut.show('p_commit')
	}
	hide_buttons() {
		this.evb.clear()
		ut.hide('p_commit')
	}
	commit() {
		let form={};
		for(let i in this.stash.ap) {
			form["ssid"+i]=this.stash.ap[i].ssid
			form["passphase"+i]=this.stash.ap[i].passphase
			form["update"+i]=this.stash.ap[i].update ? 1 : 0
		}
		ut.json('/api/ap/config',form)
		this.show()
	}
	show() {
		super.show()
		ut.hide('p_commit')
		this.cb.apply()
	}
}

export default class PagesWifi extends PLayer1 {
	constructor(){
		super()
		this.id='m_wifi'
		this.stash={
			ap: []
		}
		const sr={
			stash: this.stash,
			reset: ()=>{ this.reset() },
		}
		const pa={
			list:   new PageWifiList(sr),
			info:   new PageWifiSTAInfo(sr),
			scan:   new PageWifiSTAScan(sr),
			commit: new PageWifiSTACommit(sr),
		}
		pa.list.cb={
			edit:        (n)=>{ pa.info.show(n) },
			committable: (n)=>{ pa.commit.committable(n) },
		}
		pa.info.cb={
			back:        ( )=>{ pa.list.show() },
			scan:        (n)=>{ pa.scan.show(n) },
			apply:       ( )=>{ pa.list.show() },
		}
		pa.scan.cb={
			back:        (n)=>{ pa.info.show(n) },
			apply:       (s)=>{ pa.info.apply(s) },
		}
		pa.commit.cb={
			apply:       ( )=>{ this.leave() },
		}
		this.pa=pa
	}
	hide() {
		super.hide()
		this.pa.commit.committable(false)
	}
	pages_start() {
		this.pa.list.load()
	}
}

