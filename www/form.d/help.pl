#
#  Synopsis:
#	Write <div> help page for script form.
#  Source Path:
#	form.cgi
#  Source SHA1 Digest:
#	No SHA1 Calculated
#  Note:
#	form.d/help.pl was generated automatically by cgi2perl5.
#
#	Do not make changes directly to this script.
#

our (%QUERY_ARG);

print <<END;
<div$QUERY_ARG{id_att}$QUERY_ARG{class_att}>
END
print <<'END';
 <h1>Help Page for <code>/cgi-bin//form</code></h1>
 <div class="overview">
  <h2>Overview</h2>
  <dl>
<dt>Title</dt>
<dd>HTML Form Debugging</dd>
<dt>Synopsis</dt>
<dd>Simple form debugging.</dd>
<dt>Blame</dt>
<dd>John the Scott, jmscott@setspace.com</dd>
  </dl>
 </div>
</div>
END

1;
