// vim:ft=javascript

export default class Utils {
	constructor(w){
		this.w=w
		this.d=this.w.document
		this.stash={}
	}
	id(target) {
		return this.d.getElementById(target)
	}
	show(id) {
		console.log("show:"+id)
		this.id(id).style.display = 'block'
		return this
	}
	hide(id) {
		console.log("hide:"+id)
		this.id(id).style.display = 'none'
		return this
	}
	enable(id) {
		this.id(id).disabled=false;
	}
	disable(id) {
		this.id(id).disabled=true;
	}
	inner(id,h) {
		return h ? (this.id(id).innerHTML=h) : h;
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

