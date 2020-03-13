// ------------------------
// gulpfile.babel.js
// ------------------------

import gulp           from 'gulp'
import log            from 'fancy-log'
import webpack        from 'webpack-stream'
import sass           from 'gulp-sass'
import htmlmin        from 'gulp-htmlmin'
import ejs            from 'gulp-ejs'
import fs             from 'fs'
import rename         from 'gulp-rename'
import rimraf         from 'rimraf'

const DEVELOPMENT = process.env.NODE_ENV === 'development'
if(DEVELOPMENT) { log.info("DEVELOPMENT MODE") }

// ------------------------
// tasks
// ------------------------
gulp.task('clean',(cb)=>{
	rimraf('var',cb)
})

gulp.task('es:server',()=>{
	return gulp.src('./src/es/server.es')
	.pipe(webpacks.server('server.js'))
	.pipe(gulp.dest('var/work'))
})

gulp.task('es:client',()=>{
	return gulp.src('./src/es/client.es')
	.pipe(webpacks.client('index.js'))
	.pipe(gulp.dest('var/work'))
})

gulp.task('sass',()=>{
	return gulp.src('./src/sass/*.scss')
	.pipe(sass({ outputStyle: DEVELOPMENT ? 'expanded' : 'compressed'})
	.on('error',sass.logError))
	.pipe(gulp.dest('./var/work'))
})

gulp.task('ejs',()=>{
	return gulp.src('./src/ejs/index.ejs')
	.pipe(ejs({
		css: fs.readFileSync('./var/work/index.css'),
		js:  fs.readFileSync('./var/work/index.js'),
		production: ! DEVELOPMENT,
	}).on('error',log))
	.pipe(rename({ extname: '.html' }))
	.pipe(gulp.dest('./var/work'))
})

gulp.task('htmlmin',()=>{
	return gulp.src('./var/work/index.html')
	.pipe(htmlmin({collapseWhitespace: true}))
	.pipe(gulp.dest('./var/work/min'))
})

const do_copy=()=>{
	if(DEVELOPMENT) {
		return gulp.series(
			()=>{ return gulp.src('./var/work/server.js'          ).pipe(gulp.dest('./var/build'))        },
			()=>{ return gulp.src('./var/work/index.{js,css,html}').pipe(gulp.dest('./var/build/public')) },
		)
	} else {
		return gulp.series(
			()=>{ return gulp.src('./var/work/server.js'     ).pipe(gulp.dest('./var/build'))        },
			()=>{ return gulp.src('./var/work/min/index.html').pipe(gulp.dest('./var/build/public')) },
		)
	}
}

gulp.task('build',gulp.series('es:server','es:client','sass','ejs','htmlmin',do_copy()))
gulp.task('default',gulp.series('build'))

// ------------------------
// WebPack
// JavaScriptの連結圧縮
// ------------------------
const webpacks=(()=>{
	const common=(args)=>{
		return webpack({
			node: {
				__dirname: false,
				__filename: false,
				process: false,
			},
			devtool: DEVELOPMENT ? 'source-map' : undefined,
			mode: DEVELOPMENT ? 'development' : 'production',
			module: { rules: [{
				test: /\.es$/,
				use: {
					loader: 'babel-loader',
					options: {
						presets: [
							[ "@babel/preset-env", args.preset_env ]
						]
					}
				}
			}]},
			externals: args.server ? args.externals : [],
			output: {
				filename: args.filename,
				libraryTarget: args.server ? 'commonjs2' : undefined
			}
		})
	}
	return {
		// サーバサイドJS
		server: (filename)=>{
			return common({
				server: true,
				filename: filename,
				// node.jsのバージョン
				preset_env: { targets: { "node": "11.0" }},
				// 埋め込まないモジュール
				externals: [
					'express',
					'request',
					'cors',
					'querystring',
					'cookie-parser',
					'body-parser',
				]
			})
		},
		// クライアントサイドJS
		client: (filename)=>{
			return common({
				client: true,
				filename: filename,
				// 対象のブラウザバージョン
				preset_env: { targets: { browsers: "last 2 versions" }}
			})
		}
	}
})()


