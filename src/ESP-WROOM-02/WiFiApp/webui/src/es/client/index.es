// vim:ft=javascript
import Utils from './utils.es'
import { Events, PLayer1 } from './pages.es'
import PagesWifi from './pages_wifi.es'

class PagesMain extends PLayer1 {
	constructor(ba){
		super(ba)
		this.id='m_index'
	}
}

class RootPage {
	constructor(ba) {
		this.ba=ba;
		this.ev=new Events(ba)
		this.pa={
			'main':  { page: new PagesMain(ba), btn: 'btc_main' },
			'wifi':  { page: new PagesWifi(ba), btn: 'btc_wifi' },
		}
//		for(let i in this.pa) {
//			this.pa[i].page.leave=()=>{ this.enable_all() }
//		}
	}
	change(name) {
		for(let i in this.pa) {
			this.pa[i].page.hide()
		}
		this.pa[name].page.show()
		return this
	}
	disable_all() {
		this.ev.clear()
		for(let i in this.pa) {
			this.ba.disable(this.pa[i].btn)
		}
		return this
	}
	enable_all() {
		for(let i in this.pa) {
			this.enable(i)
		}
		return this
	}
	enable(name) {
		console.log('ENABLE',name)
		const p=this.pa[name]
		this.ba.enable(p.btn)
		this.ev.click(p.btn,()=>{ this.change(name) })
		return this
	}
}

export default class Index {
	constructor(){
		this.ba=new Utils(window)
		this.rp=new RootPage(this.ba)
	}
	run() {
		this.rp.change('main').enable_all()
		this.data_pane()
	}
	data_pane() {
		const iv=()=>{
			const av=(id,v)=>{
				v=v ? v : '---'
				this.ba.inner(id,v)
			}
			this.ba.json('/api/ap/info').then((d)=>{
				const hn='<a href="http://'+d.hostname+'.local/">'+d.hostname+'</a>'
				const apip='<a href="http://'+d.ap.ip+'/">'+d.ap.ip+'</a>'
				const staip=( d.sta.ip == '0.0.0.0' ) ? '---' : d.sta.ip
				av('dp_hostname',hn)
				av('dp_ap_ssid',d.ap.ssid)
				av('dp_ap_ip',apip)
				av('dp_sta_ssid',d.sta.ssid)
				av('dp_sta_ip',staip)
				this.ba.stash.wifi={
					hostname: d.hostname,
					ap:       d.ap,
					sta:      d.sta,
				}
			})
		}

		if(this.data_pane_timer) {
			clearInterval(this.data_pane_timer)
		}
		setInterval(iv,10*1000)
		iv();
	}
}

