// vim:ft=javascript
import Utils from './utils.es'
import { Events, PLayer1, PLayer2 } from './pages.es'
import PagesWifi from './pages_wifi.es'

const ut=new Utils(window)

class PagesMain extends PLayer1 {
	constructor(){
		super()
		this.id='m_index'
	}
}

class RootPage {
	constructor() {
		this.ev=new Events()
		this.pa={
			'main':  { page: new PagesMain(), btn: 'btc_main' },
			'wifi':  { page: new PagesWifi(), btn: 'btc_wifi' },
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
			ut.disable(this.pa[i].btn)
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
		ut.enable(p.btn)
		this.ev.click(p.btn,()=>{ this.change(name) })
		return this
	}
}

export default class Index {
	constructor(){
		this.rp=new RootPage()
	}

	run() {
		this.rp.change('main').enable_all()
		this.data_pane()
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

