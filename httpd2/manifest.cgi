<?xml version="1.0" encoding="UTF-8"?>
<cgi
	name="manifest"
 >
 <title>Manifest of Files in Web Site</title>
 <synopsis>
 Build a manifest of the web site by reading the xml *.cgi definition files.
 </synopsis>
 <subversion
 	id="$Id"
 />
 <blame>John the Scott, jmscott@setspace.com</blame>

 <GET>
 <examples>
  <example
	query="out=table">
  	Write an html &lt;table&gt; element that is a list
	of all the /cgi-bin/scripts.
  </example>
 </examples>

 <out content-type="text/html">
  <putter name="table">

   <title>Manifest of CGI Scripts as an HTML &lt;table&gt;</title>
   <synopsis>
    Generate an html &lt;table&gt; of the *.cgi xml files
    in $SERVER_ROOT/lib.
   </synopsis>
  </putter>
 </out>
</GET>
</cgi>
