// vim:ft=javascript
import Utils from './utils.es'

export class Events {
	constructor(ba){
		this.ba=ba
		this.events=[]
	}
	click(id,li) {
		this.ba.id(id).addEventListener('click',li);
		this.events.push([id,'click',li]);
		return this
	}
	_rh(e) {
		this.ba.id(e[0]).removeEventListener(e[1],e[2])
	}
	renove(id,ev) {
		for(let i in this.events) {
			const e=this.events[i];
			if((e[0] == id) && (e[1] == ev)) { this._rh(e) }
		}
	}
	clear() {
		for(let i in this.events) { this._rh(this.events[i]) }
		this.events=[]
		return this
	}
}

export class PLayer1 {
	constructor(ba){
		this.ba=ba
		this.ev=new Events(ba)
		this.cb={
			leave: ()=>{}
		}
	}
	show() {
		this.reset()
		this.pages_start()
		this.ba.show(this.id)

	}
	hide() {
		this.reset()
		this.ba.hide(this.id)
		this.cb.leave()
	}
	reset() {
		for(let i in this.pa) {
			this.pa[i].hide()
		}
	}
	leave() {}
	pages_start() {}
}

export class PLayer2 {
	constructor(ba){
		this.ba=ba
		this.reset=()=>{ this.cb.reset(); return this }
		this.ev=new Events(ba)
	}
	show(n) {
		this.number=n
		this.reset().render().events()
		this.ba.show(this.ids[0])
		return this
	}
	hide() {
		for(let i in this.ids) {
			this.ba.hide(this.ids[i])
		}
		this.ev.clear()
	}
	render(){ return this }
	events(){ return this }
}

