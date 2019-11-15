#include <stdio.h>

typedef void* HtmlT;

/* */
#define html_ctor(OBJ) \
        _html_ctor(OBJ, __FILE__, __LINE__)
HtmlT _html_ctor(const char*, const char*, unsigned);

/* */
#define html_load(OBJ) \
        _html_load(OBJ, __FILE__, __LINE__)
unsigned _html_load(HtmlT, const char *, unsigned);

struct html{
struct dictionary *dict;
HANDLE hfile;
HANDLE hmap;
void *fmap;
};

/*
HTML 5
 <!DOCTYPE HTML>

HTML 4.01
 Strict
 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
 "http://www.w3.org/TR/html4/strict.dtd">

 Transitional
 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
  "http://www.w3.org/TR/html4/loose.dtd">

 Frameset
 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN"
  "http://www.w3.org/TR/html4/frameset.dtd">

XHTML 1.0
 Strict
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

 Transitional
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

 Frameset
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Frameset//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd">

XHTML1.1 - DTD
 <!DOCTYPE html PUBLIC "-//W3C/DTD XHTML1.1//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1.dtd">

XHTML Basic 1.1
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML Basic 1.1//EN"
  "http://www.w3.org/TR/xhtml-basic/xhtml-basic1.dtd">

*/

/*
HTML Tags
<a> : charset, coords, download, href, hfrelang, media, name, ping, referrerpolicy, rel, rev, shape, target, type
*/

enum MEDIA_DEVICE_TYPE {
  ALL,
  AURAL,
  BRAILLE,
  HANDHELD,
  PROJECTION,
  SCREEN,
  PRINT,
  TTY,
  TV
}

struct data_attr {
  char *attr_name;
  char *attr_value;
};

struct global_tag {
  char accesskey;
  char *class;
  char contenteditable;

  struct data_attr *data;
  char data_attr_count; 

  char dir;
  char draggable;
  char dropzone;
  char hidden;
  char *id;
  unsigned char lang; //191 language codes
  char spellcheck;
  char *style;
  int tabindex;
  char *title;
  char translate;
};

struct anchor_media_attr {
  enum MEDIA_DEVICE_TYPE device_type;
  short width;
  short min-width;
  short max-width;

  short height;
  short min-height;
  short max-height;

  short device-width;
  short min-device-width;
  short max-device-width;

  short device-height;
  short min-device-height;
  short max-device-height; 

  char orientation; //portrait or landscape

  short aspect-ratio;
  short min-aspect-ratio;
  short max-aspect-ratio;

  short device-aspect-ratio;
  short min-device-aspect-ratio;
  short max-device-aspect-ratio;

};

struct anchor_tag {
  struct global_tag global;
  char *download;
  char *href;
  char *name;
  char *ping;
  unsigned short type;
  char target;
};

struct form_tag {
  struct global_tag global;
  char method;
  char *action; //url where to send form data
  char autocomplete;
  char enctype;
  char target;
};

struct img_tag {
  struct global_tag global;
  char *alt; //alternate text for an image
  unsigned short height;
  unsigned short width;
  char *src; //url of an image
};

struct audio_tag {
  struct global_tag global;
  char autoplay;
  char controls;
  char loop;
  char muted;
  char preload;
  char *src;
};

/*
<abbr>
<address>
<area>
<article>
<aside>
<audio>
<b>
<base>
<bdi>
<bdo>
<blockquote>
<body>
<br>
<button>
<canvas>
<caption>
<cite>
<code>
<col>
<colgroup>
<data>
<datalist>
<dd>
<del>
<details>
<dfn>
<dialog>
<div>
<dl>
<dt>
<em>
<embed>
<fieldset>
<figure>
<footer>
<form>
<h1>
<h2>
<h3>
<h4>
<h5>
<h6>
<head>
<header>
<hgroup>
<hr>
<html>
<i>
<iframe> //webkitAllowFullScreen mozallowfullscreen allowFullScreen
<img>
<input>
<ins>
<kbd>
<keygen>
<label>
<legend>
<li>
<link>
<main>
<map>
<mark>
<menu>
<menuitem>
<meta>
<meter>
<nav>
<noscript>
<object>
<ol>
<optgroup>
<option>
<output>
<p>
<param>
<pre>
<progress>
<q>
<rb>
<rp>
<rt>
<rtc>
<ruby>
<s>
<samp>
<script>
<section>
<select>
<small>
<source>
<span>
<strong>
<style>
<sub>
<summary>
<sup>
<table>
<tbody>
<td>
<template>
<textarea>
<tfoot>
<th>
<thead>
<time>
<title>
<tr>
<track>
<u>
<ul>
<var>
<video>
<wbr>

HtmlT _html_ctor(const char *file, const char *_file, unsigned _line)
{
struct html *htmlo;

    htmlo = malloc(sizeof(struct html));
    assert(htmlo);

    htmlo->dict = dict_ctor(malloc, free);
    assert(htmlo->dict);

    jso->fmap = create_filemap(file, &htmlo->hfile, &htmlo->hmap);
    return jso;
}


}

JsonT _json_ctor(const char *file, const char *_file, unsigned _line)
{
struct json *jso;

    jso = malloc(sizeof(struct json));
    assert(jso);

    jso->dict = dict_ctor(malloc, free);
    assert(jso->dict);

    jso->fmap = create_filemap(file, &jso->hfile, &jso->hmap);
    return jso;
}



int main(int argc, char **argv)
{
    
    return 1;
}
