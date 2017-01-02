<cgi
	name="httpd2"
>
 <title>HTTPD2 CGI Default Elements</title>
 <GET>
  <out>
   <query-args>
    <arg
   	name="qarg"
	perl5_re="\w[\w.]{0,15}"
    >
     <title>Query Argument of URI</title>
     <synopsis>
      The name of an argument in the query portion of a URL.
     </synopsis>
   </arg>
   <arg
   	name="evar"
	perl5_re="\w[\w.]{0,31}"
   >
    <title>Environment Variable in UNIX Process</title>
    <synopsis>
     The name of an an environment variable defined in the unix
     process environment.
    </synopsis>
   </arg>
   <arg
    	name="inro"
	perl5_re="yes|no|wo"
   >
    <title>Read Only</title>
    <synopsis>
     Set readonly attribute in &lt;input&gt; element.
     If the value is &quot;wo&quot;,
     means set readonly only when a the value attribute is
     a non zero length.
    </synopsis>
   </arg>
   <arg
    	name="inty"
	perl5_re="text|hidden|password"
    >
     <title>&lt;input&gt; Type</title>
   </arg>

   <arg
	name="path"
	perl5_re="[\w][\w/.-]{0,128}"
    >
    <title>File Path</title>
    <synopsis>
   	The file system path to the file, relative to 
	the SERVER_ROOT environment variable.
	The file name can not contain .. or /.
	The perl5 regular expression does NOT check this.
    </synopsis>
   </arg>

   </query-args>
  </out>
 </GET>
</cgi>
