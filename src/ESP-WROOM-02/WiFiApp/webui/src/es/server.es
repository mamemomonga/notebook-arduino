import express from 'express'
import request from 'request'
import cors from 'cors'
import querystring from 'querystring'
import bodyParser from 'body-parser'

const serve_port=process.env.SERVE_PORT ? process.env.SERVE_PORT : 8888
const app = express()

app.use(express.static(`${__dirname}/public`))

app.use(bodyParser.urlencoded({ extended: false }))
app.use(bodyParser.json())

app.get('/api/page', (req, res)=>{
	res.send('{ "error": 0,"page": "index" }')
})

app.get('/api/ap/list', (req, res)=>{
	console.log(req.body)
	const data={
		error: 0,
		data: [
			{ ssid: "SSID1" },
			{ ssid: "" },
			{ ssid: "" },
			{ ssid: "" },
		]
	};
	res.send(JSON.stringify(data))
})

app.post('/api/ap/scan', (req, res)=>{
	res.send('{ "error": 0 }')
})

app.get('/api/ap/scan', (req, res)=>{
	console.log(req.body)
	const data={
		error: 0,
		data: [
			{ ssid: "SSID1", channel: "1", rssi: "-60" },
			{ ssid: "SSID2", channel: "2", rssi: "-60" },
			{ ssid: "SSID3", channel: "3", rssi: "-60" },
			{ ssid: "SSID4", channel: "4", rssi: "-60" },
			{ ssid: "SSID5", channel: "5", rssi: "-60" },
		]
	};
	setTimeout(()=>{
		res.send(JSON.stringify(data))
	},3000)
})

app.post('/api/ap/config', (req, res)=>{
	console.log(req.body)
	res.send('{ "error": 0 }')
})

app.post('/api/ap/reset', (req, res)=>{
	console.log(req.body)
	res.send('{ "error": 0 }')
})


console.log(`Listening on ${serve_port}`)
app.listen(serve_port)

