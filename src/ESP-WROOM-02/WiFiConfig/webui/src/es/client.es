// vim:ft=javascript
import Index from './client/index.es'
import Utils from './client/utils.es'

document.addEventListener('DOMContentLoaded',()=>{
	const u=new Utils(window);
	u.json('/api/page').then((d)=>{
		if(d.page="index") {
			new Index(window).run()
		}
	});
})

