// vim:ft=javascript
import Utils from './utils.es'
const ut=new Utils(window)

export class Events {
	constructor(){
		this.events=[]
	}
	click(id,li) {
		const el=ut.id(id);
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

export class Pages {
	constructor(p){
		this.stash=p.stash
		this.reset=()=>{ p.reset(); return this }
		this.ev=new Events()
	}
	show(n) {
		this.number=n
		this.reset().render().events()
		ut.show(this.ids[0])
		return this
	}
	hide() {
		for(let i in this.ids) {
			ut.hide(this.ids[i])
		}
		this.ev.clear()
	}
	render(){ return this }
	events(){ return this }
}

