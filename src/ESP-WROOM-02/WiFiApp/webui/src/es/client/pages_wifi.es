// vim:ft=javascript
import Utils from './utils.es'
import { Events, PLayer1, PLayer2 } from './pages.es'
const ut=new Utils(window)

class PageWifiList extends PLayer2 {
	constructor(ba){
		super(ba)
		this.ids=['d_sta_list'];
		this.cb={
			edit: (n)=>{},
		}
	}
	load() {
		this.ba.json('/api/ap/list')
		.then((d)=>{
			for(let i in d.data) {
				this.ba.stash.ap[i]={
					ssid:      d.data[i].ssid,
					passphase: '',
					update:    false,
				}
			}
		}).then(()=>{
			this.cb.committable(false)
			return this.show();
		})
	}
	events() {
		for(let i in this.ba.stash.ap) {((j)=>{
			this.ev.click('b_sta_setup_'+j,(e)=>{ this.cb.edit(j) })
		})(i)}
		return this
	}
	render() {
		let html='<tr><th>番号</th><th style="min-width:200px">SSID</th><th>　</th></tr>';
		let update=false;
		const s=this.ba.stash.ap
		for(let i in s) {
			const v=s[i]
			html=html+'<tr>'
				+'<td>'+(new Number(i)+1)+'</td>'
				+ ( v.update ? '<td style="background-color:#FFFFAA">' : '<td>' ) +v.ssid+'</td>'
				+'<td><button id="b_sta_setup_'+i+'" class="bt_blue">設定</button></td>';
				+'</tr>';
			if(v.update) { update=true }
		}
		if(update) this.cb.committable(true)
		this.ba.inner('t_sta_list',html);
		return this;
	}
}

class PageWifiSTAInfo extends PLayer2 {
	constructor(ba){
		super(ba)
		this.ids=['d_sta_info']
		this.cb={
			back:   ( )=>{},
			scan:   (n)=>{},
			apply: ( )=>{},
		}
	}
	render() {
		const s=this.ba.stash.ap[this.number]
		this.ba.inner('s_sta_info_num',(new Number(this.number)+1));
		this.ba.id('t_sta_info_ssid').value=s.ssid
		this.ba.id('t_sta_info_passphase').value=s.passphase
		return this
	}
	apply(ssid) {
		this.reset().render().events()
		this.ba.id('t_sta_info_ssid').value=ssid
		this.ba.show(this.ids[0])
		return this
	}
	events() {
		this.ev.click('b_sta_info_back',()=>{ this.cb.back() })
		this.ev.click('b_sta_info_scan',()=>{ this.cb.scan(this.number) })
		this.ev.click('b_sta_info_commit',()=>{
			const ssid=this.ba.id('t_sta_info_ssid').value
			const passphase=this.ba.id('t_sta_info_passphase').value
			this.ba.stash.ap[this.number]={ ssid: ssid, passphase: passphase, update: true }
			this.cb.apply()
		})
		return this
	}
}
class PageWifiSTAScan extends PLayer2 {
	constructor(ba){
		super(ba)
		this.ids=['d_sta_scan','d_sta_scan_now']
		this.cb={
			back:  ( )=>{},
			apply: ( )=>{},
		}
		this.d={};
	}
	fetch() {
		return new Promise((resolve,reject)=>{
			this.ba.json('/api/ap/scan',{"fetch":1}).then(()=>{
				setTimeout(()=>{
					this.ba.json('/api/ap/scan').then((d)=>{
						this.d=d.data;
						resolve()
					}).catch((e)=>{ reject(e) });
				},1000)
			}).catch((e)=>{ reject(e) })
		})
	}
	show(n) {
		this.number=n
		this.ba.inner('s_sta_scan_num',(new Number(n)+1));
		this.reset()
		this.ba.show('d_sta_scan_now');
		this.fetch().then(()=>{
			this.reset().render().events()
			this.ba.hide('d_sta_scan_now').show(this.ids[0])
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
		this.ba.inner('t_sta_scan',html);
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
	constructor(ba){
		super(ba)
		this.ids=['d_sta_commit']
		this.cb={
			apply:  ()=>{},
		}
		this.evb=new Events(ba);
		this.committable(false)
	}
	committable(n) {
		n ? this.show_buttons() : this.hide_buttons()
	}
	show_buttons() {
		this.evb.click('b_commit',()=>{ this.commit() })
		this.ba.show('p_commit')
	}
	hide_buttons() {
		this.evb.clear()
		this.ba.hide('p_commit')
	}
	commit() {
		let form={};
		const s=this.ba.stash.ap
		for(let i in s) {
			form["ssid"+i]=s[i].ssid
			form["passphase"+i]=s[i].passphase
			form["update"+i]=s[i].update ? 1 : 0
		}
		this.ba.json('/api/ap/config',form)
		this.show()
	}
	show() {
		super.show()
		this.ba.hide('p_commit')
		this.cb.apply()
	}
}

export default class PagesWifi extends PLayer1 {
	constructor(ba){
		super(ba)
		this.id='m_wifi'
		ba.stash={
			ap: [],
		}
		const pa={
			list:   new PageWifiList(ba),
			info:   new PageWifiSTAInfo(ba),
			scan:   new PageWifiSTAScan(ba),
			commit: new PageWifiSTACommit(ba),
		}
		const rl=()=>{ this.reset() }
		pa.list.cb={
			reset: ( )=>{ rl() },
			edit:  (n)=>{ pa.info.show(n) },
			committable: (n)=>{ pa.commit.committable(n) },
		}
		pa.info.cb={
			reset: ( )=>{ rl() },
			back:  ( )=>{ pa.list.show() },
			scan:  (n)=>{ pa.scan.show(n) },
			apply: ( )=>{ pa.list.show() },
		}
		pa.scan.cb={
			reset: ( )=>{ rl() },
			back:  (n)=>{ pa.info.show(n) },
			apply: (s)=>{ pa.info.apply(s) },
		}
		pa.commit.cb={
			reset: ( )=>{ rl() },
			apply: ( )=>{ this.leave() },
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

