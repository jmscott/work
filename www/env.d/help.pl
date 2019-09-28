#
#  Synopsis:
#	Write <div> help page for script env.
#  Source Path:
#	env.cgi
#  Source SHA1 Digest:
#	No SHA1 Calculated
#  Note:
#	env.d/help.pl was generated automatically by cgi2perl5.
#
#	Do not make changes directly to this script.
#

our (%QUERY_ARG);

print <<END;
<div$QUERY_ARG{id_att}$QUERY_ARG{class_att}>
END
print <<'END';
 <h1>Help Page for <code>/cgi-bin/env</code></h1>
 <div class="overview">
  <h2>Overview</h2>
  <dl>
<dt>Title</dt>
<dd>Unix Process Environment</dd>
<dt>Synopsis</dt>
<dd>Build html elements doing simple string transformations of 
 values in the unix process environment.</dd>
<dt>Blame</dt>
<dd>John the Scott, jmscott@setspace.com</dd>
  </dl>
 </div>
 <div class="GET">
  <h2><code>GET</code> Request.</h2>
   <div class="out">
    <div class="handlers">
    <h3>Output Scripts in <code>$SERVER_ROOT/lib/env.d</code></h3>
    <dl>
     <dt>text - HTML Text</dt>
     <dd>
<div class="query-args">
 <h4>Query Args</h4>
 <dl>
  <dt>evar</dt>
  <dd>
   <ul>
    <li><code>inherit:</code> yes</li>
    <li><code>perl5_re:</code> .{1,64}</li>
   </ul>
  </dd>
  <dt>qarg</dt>
  <dd>
   <ul>
    <li><code>inherit:</code> yes</li>
    <li><code>perl5_re:</code> .{1,64}</li>
   </ul>
  </dd>
  </dl>
</div>
     </dd>
     <dt>dl - HTML &#60;dl&#62; of Process Environment Variables</dt>
     <dd>
     </dd>
     <dt>input - HTML &#60;input&#62; of Query Argument or Process Enviroment</dt>
     <dd>
<div class="query-args">
 <h4>Query Args</h4>
 <dl>
  <dt>name - Name of &#60;input&#62; attribute</dt>
  <dd>
   <ul>
    <li><code>perl5_re:</code> \w{1,16}</li>
   </ul>
  </dd>
  <dt>inro</dt>
  <dd>
   <ul>
    <li><code>inherit:</code> yes</li>
   </ul>
  </dd>
  <dt>type</dt>
  <dd>
   <ul>
    <li><code>inherit:</code> yes</li>
    <li><code>perl5_re:</code> text|hidden|submit</li>
    <li><code>required:</code> yes</li>
   </ul>
  </dd>
  <dt>evar</dt>
  <dd>
   <ul>
    <li><code>perl5_re:</code> .{1,64}</li>
   </ul>
  </dd>
  <dt>qarg</dt>
  <dd>
   <ul>
    <li><code>perl5_re:</code> .{1,64}</li>
    <li><code>required:</code> yes</li>
   </ul>
  </dd>
  </dl>
</div>
     </dd>
  </dl>
 </div>
</div>
<div class="examples">
 <h3>Examples</h3>
 <dl>
   <dt><a href="/cgi-bin/env?out=text&evar=REQUEST_METHOD">/cgi-bin/env?out=text&#38;evar=REQUEST_METHOD</a></dt>
   <dd>Write a text chunk that is the value of the REQUEST_METHOD 
	environment variable.</dd>
   <dt><a href="/cgi-bin/env?out=input&qarg=dog&dog=bark&id=pet&type=text&inro=yes">/cgi-bin/env?out=input&#38;qarg=dog&#38;dog=bark&#38;id=pet&#38;type=text&#38;inro=yes</a></dt>
   <dd>Write an html &#60;input&#62; element using the query argument
	dog for the value of the input.  The type attribute is set to text and
	the readonly=&#34;readonly&#34; attribute is set.</dd>
   <dt><a href="/cgi-bin/env?out=dl&class=system&id=process-env">/cgi-bin/env?out=dl&#38;class=system&#38;id=process-env</a></dt>
   <dd>Write out a &#60;dl&#62; of all the environment variables, 
  tagged with id &#34;process-env&#34; and class &#34;system&#34;</dd>
   <dt><a href="/cgi-bin/env?out=text&qarg=foo&foo=Bar">/cgi-bin/env?out=text&#38;qarg=foo&#38;foo=Bar</a></dt>
   <dd>Write a text chunk that is the value of the foo query argument.</dd>
 </dl>
</div>
 </div>
</div>
END

1;
