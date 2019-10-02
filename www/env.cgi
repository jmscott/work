<?xml version="1.0" encoding="UTF-8"?>
<cgi
	name="env"
 >
 <title>Unix Process Environment</title>

 <synopsis>
   Build html elements doing simple string transformations of 
   values in the apache/unix process environment.
 </synopsis>
 <blame>John the Scott, jmscott@setspace.com</blame>

 <GET>
 <examples>
  <example query="out=text&amp;name=REQUEST_METHOD">
    Write a text chunk that is the value of the REQUEST_METHOD environment
    variable.
  </example>

  <example query=
    "out=input&amp;name=dog&amp;src=QS&amp;dog=bark&amp;id=pet&amp;type=text&amp;ro=yes"
  >
    Write an html &lt;input&gt; element of type "text", seeding the input value
    from the query argument &quot;dog&quot; parsed from the
    environment variable QUERY_STRING.  The type attribute is set to
    &quot;text&quot; and the readonly=&quot;yes&quot; attribute is set.
  </example>

  <example query=
    "out=input&amp;src=QS&amp;type=hidden&amp;name=state&amp;state=TX">

    Write an html &lt;input&gt; element of type "hidden" seeded the
    "value" attribute with the value of the "state" query argument.
    Typically type "hidden" is used forms &lt;form&gt; elements needing
    query arguments.
  </example>

  <example query="out=dl&amp;class=system&amp;id=process-env">
    Write out a &lt;dl&gt; of all the environment variables, 
    tagged with id &quot;process-env&quot; and class &quot;system&quot;
  </example>

  <example query="out=text&amp;name=foo&amp;foo=Bar">
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
    	name="name"
	perl5_re="\w{1,64}"
	required="yes"
    />
    <arg
    	name="src"
	perl5_re = "env|QS"
	default="env"
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
     the value of a query argument in the env variable
     QUERY_STRING.  Query arguments in QUERY_STRING are expected
     to be separated by the &amp; character. The type argument
     specifys the value of the type attribute.  If no name is given,
     then the id attribute is used.
   </synopsis>
   <query-args>
    <arg
      name="iname"
      perl5_re="\w{1,16}"
    >
     <title>Name of &lt;input&gt; attribute</title>
     <synopsis>
       The name of the &lt;input&gt; element.  If the name
       attribute is not specified, then the value of &quot;id&quot;
       attribute is used.  If the &quot;id&quot; is not defined, then
       the value of the environment variable or query argument is used.
     </synopsis>
    </arg>
    <arg
    	name="ro"
	perl5_re="yes|no"
	default="no"
    />
    <arg
    	name="type"
	perl5_re="text|hidden|submit"
	required="yes"
    />
    <arg
    	name="name"
	perl5_re="\w{1,64}"
    />
    <arg
    	name="src"
	perl5_re="env|QS"
	default="env"
    />
   </query-args>
  </putter>
 </out>
</GET>
</cgi>
