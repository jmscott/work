<?xml version="1.0" encoding="UTF-8"?>
<cgi
	name="file">
 <subversion
 	id="$Id"
 />
 <title>Unix File System View</title>
 <synopsis>
 View files underneath $SERVER_ROOT directory.
 </synopsis>
 <blame>John the Scott, jmscott@setspace.com</blame>
 <GET>
 <examples>
  <example
	query="out=pre&amp;path=lib/file.cgi">
  	Write the file $SERVER_ROOT/lib/file.cgi
	as an <code>&lt;pre&gt;</code> html element.
  </example>
 </examples>

 <out
	content-type="text/html">
  <query-args>
   <arg
	name="path"
	required="yes"
	inherit="yes"
   />
  </query-args>
  <putter
	name="pre">
   <title>&lt;pre&gt; Value of Environment Variable</title>
   <synopsis>
    Write the value of the process environment variable or query
    argument as an html <code>&lt;pre&gt;</code> element.
   </synopsis>
  </putter>
  <putter
	name="text.full-path">
  <title>Full Path of File as HTML Text</title>
   <synopsis>
    Write the the full path of a file as html text.
   </synopsis>
  </putter>

 </out>
</GET>
</cgi>
