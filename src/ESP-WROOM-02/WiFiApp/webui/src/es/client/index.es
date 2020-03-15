// vim:ft=javascript
import Utils from './utils.es'
import { Events, Pages } from './pages.es'

const ut=new Utils(window)

class PaAPList extends Pages {
	constructor(p){
		super(p)
		this.ids=['d_ap_list'];
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
		ut.inner('t_ap_list',html);
		return this;
	}
}

class PaAPInfo extends Pages {
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
		ut.inner('s_ap_info_num',(new Number(this.number)+1));
		ut.id('t_ap_info_ssid').value=this.stash.ap[this.number].ssid
		ut.id('t_ap_info_passphase').value=this.stash.ap[this.number].passphase
		return this
	}
	apply(ssid) {
		this.reset().render().events()
		ut.id('t_ap_info_ssid').value=ssid
		ut.show(this.ids[0])
		return this
	}
	events() {
		this.ev.click('b_ap_info_back',()=>{ this.cb.back() })
		this.ev.click('b_ap_info_scan',()=>{ this.cb.scan(this.number) })
		this.ev.click('b_ap_info_commit',()=>{
			const ssid=ut.id('t_ap_info_ssid').value
			const passphase=ut.id('t_ap_info_passphase').value
			this.stash.ap[this.number]={ ssid: ssid, passphase: passphase, update: true }
			this.cb.apply()
		})
		return this
	}
}
class PaAPScan extends Pages {
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
		ut.inner('s_ap_scan_num',(new Number(n)+1));
		this.reset()
		ut.show('d_ap_scan_now');
		this.fetch().then(()=>{
			this.reset().render().events()
			ut.hide('d_ap_scan_now').show(this.ids[0])
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
		ut.inner('t_ap_scan',html);
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

class PaAPCommit extends Pages {
	constructor(p){
		super(p)
		this.ids=['d_ap_commit']
		this.cb={
			cancel: ()=>{},
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
		this.evb.click('b_cancel',()=>{ this.cb.cancel() })
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

class PWiFi {
	constructor(){
		this.stash={
			ap: []
		}
		this.ev=new Events()
	}
	pages() {
		const sr={
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
			cancel:      ( )=>{ pa.list.load() },
			apply:       ( )=>{ this.button(true) },
		}
		this.pa=pa
	}
	show() {
		this.pages()
		this.pa.list.load()
		this.button(false)
		this.reset()
		ut.show('m_wifi')

	}
	hide() {
		this.reset()
		ut.enable('btc_wifi')
		ut.hide('w_wifi')
	}
	reset() {
		for(let i in this.pa) {
			this.pa[i].hide()
		}
	}
	button(m) {
		const n='btc_wifi'
		if(m) {
			this.ev.click(n,()=>{ this.show() })
			ut.enable(n)
		} else {
			ut.disable(n)
			this.ev.clear()
		}
	}
}

export default class Index {
	constructor(){
		this.wifi=new PWiFi()
		this.ev=new Events()
	}
	run() {
		this.data_pane()
		this.wifi.button(true)
	}
	data_pane() {
		const iv=()=>{
			const av=(id,v)=>{
				v=v ? v : '---'
				ut.inner(id,v)
			}
			ut.json('/api/ap/info').then((d)=>{
				av('dp_hostname',d.hostname)
				av('dp_ap_ssid',d.ap.ssid)
				av('dp_ap_ip',d.ap.ip)
				av('dp_sta_ssid',d.sta.ssid)
				av('dp_sta_ip',d.sta.ip)
			})
		}

		if(this.data_pane_timer) {
			clearInterval(this.data_pane_timer)
		}
		setInterval(iv,10*1000)
		iv();
	}
}

