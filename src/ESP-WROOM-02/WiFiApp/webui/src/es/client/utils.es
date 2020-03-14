// vim:ft=javascript

export default class Utils {
	constructor(window_obj){
		this.w=window_obj
		this.d=this.w.document
		this.events=[];
	}
	id(target) {
		return this.d.getElementById(target)
	}
	click(id,li) {
		const el=this.id(id);
		el.addEventListener('click',li);
		this.events.push([el,'click',li]);
		return this
	}
	clear_events() {
		for(let i in this.events) {
			const e=this.events[i];
			e[0].removeEventListener(e[1],e[2])
		}
		this.events=[]
		return this
	}
	show(id) {
		this.id(id).style.display = 'block'
		return this
	}
	hide(id) {
		this.id(id).style.display = 'none'
		return this
	}
	json(url,data){
		return new Promise((resolve,reject)=>{
			const request = new XMLHttpRequest();
			if(data) {
				console.log("Fetch(POST): ",url)
				request.open('POST', url, true)
				request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8')
			} else {
				console.log("Fetch(GET): ",url)
				request.open('GET', url, true)
			}
			request.onload=function(){
				if (this.status >= 200 && this.status < 400) {
					resolve(JSON.parse(this.response))
				} else {
					reject()
				}
			}
			if(data) {
				let params=[]
	    		for( let name in data ) {
					const value = data[ name ]
					const param = encodeURIComponent( name )+'='+encodeURIComponent( value )
					params.push( param )
				}
				const d=params.join( '&' ).replace( /%20/g, '+' )
				request.send(d)
			} else {
				request.send()
			}
		})
	}
}
