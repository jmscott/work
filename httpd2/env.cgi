<?xml version="1.0" encoding="UTF-8"?>
<cgi
	name="env"
 >
 <title>Unix Process Environment</title>

 <synopsis>
 Build html elements doing simple string transformations of 
 values in the unix process environment.
 </synopsis>
 <subversion
 	id="$Id"
 />
 <blame>John the Scott, jmscott@setspace.com</blame>

 <GET>
 <examples>
  <example
	query="out=text&amp;evar=REQUEST_METHOD">
  	Write a text chunk that is the value of the REQUEST_METHOD 
	environment variable.
  </example>
  <example
	query=
"out=input&amp;qarg=dog&amp;dog=bark&amp;id=pet&amp;type=text&amp;inro=yes&amp;inty=text"
  >
  	Write an html &lt;input&gt; element using the query argument
	dog for the value of the input.  The type attribute is set to text and
	the readonly=&quot;readonly&quot; attribute is set.
  </example>

  <example
	query="out=dl&amp;class=system&amp;id=process-env">
  Write out a &lt;dl&gt; of all the environment variables, 
  tagged with id &quot;process-env&quot; and class &quot;system&quot;
  </example>

  <example
	query="out=text&amp;qarg=foo&amp;foo=Bar">
  Write a text chunk that is the value of the foo query argument.
  </example>

 </examples>

 <out content-type="text/html">

  <putter name="text">
   <title>HTML Text</title>
   <synopsis>
    Write the value of the process environment variable or query
    argument as an html text element.
   </synopsis>
   <query-args>
    <arg
    	name="evar"
	inherit="yes"
    />
    <arg
    	name="qarg"
	inherit="yes"
    />
  </query-args>
  </putter>

  <putter name="dl">
   <title>HTML &lt;dl&gt; of Process Environment Variables</title>
   <synopsis>
    Write an html definition list, &lt;dt&gt; of all the 
    environment variables in the CGI process environment,
    ordered alphabetically.
   </synopsis>
  </putter>

  <putter name="input">
   <title>HTML &lt;input&gt; of Query Argument or Process Enviroment</title>
   <synopsis>
    Write an html &lt;input&gt; element seeded with
    the value of a query argument.  Query arguments are expected
    to be separated by the &amp; character. The type argument
    specifys the value of the type attribute.  If no name is given,
    then the id attribute is used.
   </synopsis>
   <query-args>
    <arg
    	name="name"
	perl5_re="\w{1,16}"
    >
     <title>Name of &lt;input&gt; attribute</title>
     <synopsis>
      The name of the &lt;input&gt; element.  If the name
      attribute is not specified,
      then the value of &quot;id&quot; attribute is used.
      If the &quot;id&quot; is not defined, then the value of the
      &quot;qarg&quot; or &quot;evar&quot; is used.
     </synopsis>
    </arg>
    <arg
    	name="inro"
	inherit="yes"
    />
    <arg
    	name="inty"
	inherit="yes"
	required="yes"
    />
    <arg
    	name="evar"
	inherit="yes"
    />
    <arg
    	name="qarg"
	inherit="yes"
    />
   </query-args>
  </putter>
 </out>
</GET>
</cgi>
