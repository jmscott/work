<?xml version="1.0" encoding="UTF-8"?>

<cgi name="form">
 <title>HTML Form Debugging</title>
 <synopsis>Simple form debugging.</synopsis>
 <blame>John the Scott, jmscott@setspace.com</blame>

 <POST>
  <in>
   <putter name="post.dump">
    <title>Dump an HTTP POST Request</title>
    <synopsis>
      Write a &lt;div&gt; element that contains the process environment and the
      parsed posted data.  Helpful when debuging an existing form.  Just add or
      modify the following attributes to your &lt;form&gt; element
      &lt;form action=&quot;env&quot; name=&quot;div.dump&quot;.
      Be sure that the attribute method=&quot;POST&quot; is set or your browser
      will probably do a GET http request.
    </synopsis>
   </putter>

   <putter name="religion">
    <title>Simple Religion Test</title>
    <synopsis>
      This form demonstrates validation of posted values against the patterns
      defined in perl5_re.
    </synopsis>
    <vars>
     <var
       name="religion"
       required="yes"
       perl5_re="jewish|christian|muslim|hindu|buddhist|jain|agnostic|atheist"
     />
    </vars>
   </putter>
  </in>
 </POST>
</cgi>
