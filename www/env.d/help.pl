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
 <h1>Help Page for <code>/cgi-bin//env</code></h1>
 <div class="overview">
  <h2>Overview</h2>
  <dl>
<dt>Title</dt>
<dd>Unix Process Environment</dd>
<dt>Synopsis</dt>
<dd>Build html elements doing simple string transformations of 
   values in the apache/unix process environment.</dd>
<dt>Blame</dt>
<dd>John the Scott, jmscott@setspace.com</dd>
  </dl>
 </div>
 <div class="GET">
  <h2><code>GET</code> Request.</h2>
   <div class="out">
    <div class="handlers">
    <h3>Output Scripts in <code>$SERVER_ROOT/lib//env.d</code></h3>
    <dl>
     <dt>text - HTML Text</dt>
     <dd>
<div class="query-args">
 <h4>Query Args</h4>
 <dl>
  <dt>name</dt>
  <dd>
   <ul>
    <li><code>perl5_re:</code> \w{1,64}</li>
    <li><code>required:</code> yes</li>
   </ul>
  </dd>
  <dt>src</dt>
  <dd>
   <ul>
    <li><code>default:</code> env</li>
    <li><code>perl5_re:</code> env|QS</li>
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
  <dt>iname - Name of &#60;input&#62; attribute</dt>
  <dd>
   <ul>
    <li><code>perl5_re:</code> \w{1,16}</li>
   </ul>
  </dd>
  <dt>ro</dt>
  <dd>
   <ul>
    <li><code>default:</code> no</li>
    <li><code>perl5_re:</code> yes|no</li>
   </ul>
  </dd>
  <dt>type</dt>
  <dd>
   <ul>
    <li><code>perl5_re:</code> text|hidden|submit</li>
    <li><code>required:</code> yes</li>
   </ul>
  </dd>
  <dt>name</dt>
  <dd>
   <ul>
    <li><code>perl5_re:</code> \w{1,64}</li>
   </ul>
  </dd>
  <dt>src</dt>
  <dd>
   <ul>
    <li><code>default:</code> env</li>
    <li><code>perl5_re:</code> env|QS</li>
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
   <dt><a href="/cgi-bin/env?out=text&name=REQUEST_METHOD">/cgi-bin/env?out=text&#38;name=REQUEST_METHOD</a></dt>
   <dd>Write a text chunk that is the value of the REQUEST_METHOD environment
    variable.</dd>
   <dt><a href="/cgi-bin/env?out=input&name=dog&src=QS&dog=bark&id=pet&type=text&ro=yes">/cgi-bin/env?out=input&#38;name=dog&#38;src=QS&#38;dog=bark&#38;id=pet&#38;type=text&#38;ro=yes</a></dt>
   <dd>Write an html &#60;input&#62; element of type &#34;text&#34;, seeding the input value
    from the query argument &#34;dog&#34; parsed from the
    environment variable QUERY_STRING.  The type attribute is set to
    &#34;text&#34; and the readonly=&#34;yes&#34; attribute is set.</dd>
   <dt><a href="/cgi-bin/env?out=input&src=QS&type=hidden&name=state&state=TX">/cgi-bin/env?out=input&#38;src=QS&#38;type=hidden&#38;name=state&#38;state=TX</a></dt>
   <dd>Write an html &#60;input&#62; element of type &#34;hidden&#34; seeded the
    &#34;value&#34; attribute with the value of the &#34;state&#34; query argument.
    Typically type &#34;hidden&#34; is used forms &#60;form&#62; elements needing
    query arguments.</dd>
   <dt><a href="/cgi-bin/env?out=dl&class=system&id=process-env">/cgi-bin/env?out=dl&#38;class=system&#38;id=process-env</a></dt>
   <dd>Write out a &#60;dl&#62; of all the environment variables, 
    tagged with id &#34;process-env&#34; and class &#34;system&#34;</dd>
   <dt><a href="/cgi-bin/env?out=text&name=foo&foo=Bar&src=QS">/cgi-bin/env?out=text&#38;name=foo&#38;foo=Bar&#38;src=QS</a></dt>
   <dd>Write a text chunk that is the value of the foo query argument.</dd>
 </dl>
</div>
 </div>
</div>
END

1;
