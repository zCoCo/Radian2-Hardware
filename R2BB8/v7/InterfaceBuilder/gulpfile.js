/****
 * gulpfile for managing dev and build.
 *
 * Authors: Connor W. Colombo (Ottia)
****/

const gulp  = require('gulp');
const path = require('path');

const jshint = require('gulp-jshint');
const concat = require('gulp-concat');
const uglify = require('gulp-uglify');
const rename = require('gulp-rename');
const babel = require('gulp-babel');
const htmlreplace = require('gulp-html-replace');

// Important Directories:
var dirs = {
  // Source:
  HTML: ['index.html'],
  CSS: ['src/css/*.css'],
  JS: ['src/js/*.js', 'src/js/**/*.js'],
  JSX: ['src/jsx/*.jsx', 'src/jsx/**/*.jsx'],
  // Output:
  MINIFIED_JS: 'build.min.js',
  DIST_SRC: 'dist/src',
  DIST_BUILD: 'dist/build',
  DIST: 'dist' // FIGURE OUT index.html REFS for DIST/BUILD vs DIST/SRC. Move index?
};

// Lint Task
gulp.task('lint', function(){
  return gulp.src(dirs.JS)
    .pipe(jshint())
    .pipe(jshint.reporter('jshint-stylish'));
});

// Concatenate & Minify JS and JSX
gulp.task('condense-js', function(){
  return gulp.src(dirs.JS.concat(dirs.JSX))
    .pipe(babel())
    .pipe(concat(dirs.MINIFIED_JS))
    .pipe(uglify(dirs.MINIFIED_JS))
    .pipe(gulp.dest(dirs.DIST_BUILD));
});

// Transform JSX to JS:
gulp.task('transform', function(){
  return gulp.src(dirs.JSX)
    .pipe(babel())
    .pipe(gulp.dest(dirs.DIST_SRC));
});

// Migrate Non-Transformed Code to Distribution Source Directory:
gulp.task('migrate-to-dist', function(){
  gulp.src(dirs.CSS)
    .pipe(gulp.dest(dirs.DIST_SRC));
  gulp.src(dirs.HTML)
    .pipe(gulp.dest(dirs.DIST));
});

// Migrate Non-Transformed Code to Distribution Build Directory:
gulp.task('migrate-to-build', function(){
  gulp.src(dirs.CSS)
    .pipe(gulp.dest(dirs.DIST_BUILD));
});

// Update References in HTML to dist directory:
gulp.task('updateHTML', function(){
  return gulp.src(dirs.HTML)
    .pipe(htmlreplace({
      js: MINIFIED_JS
    }))
    .pipe(gulp.dest(dirs.DIST_BUILD));
});

// Watch for Changes:
gulp.task('watch', function(){
  return gulp.watch(
    dirs.BASE
      .concat(dirs.CSS)
      .concat(dirs.JS)
      .concat(dirs.JSX),
    ['lint', 'transform', 'migrate-to-dist']
  );
});

// Watch by Default:
gulp.task('default', ['watch']);

gulp.task('build', ['lint', 'condense-js', 'migrate-to-build', 'updateHTML']);
