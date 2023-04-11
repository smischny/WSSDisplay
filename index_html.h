#ifndef SETUP_SERIAL_HTML__H
#define SETUP_SERIAL_HTML__H

static const char indexHtml [] PROGMEM = R"~(
<!DOCTYPE HTML PUBLIC - "-//W3C//DTD HTML 3.2//EN">
<HTML>
<HEAD>
  <meta charset="UTF-8">
  <meta name="description" content="Web Display">
  <meta name="keywords" content="Display">
  <meta name="author" content="Toby Smischny">
<TITLE>Web Display</TITLE>
</HEAD>
<BODY onload="init('ws://'+window.location.hostname+':81/display')"  BGCOLOR="#FFFFFF" TEXT="#000000" LINK="#0000FF" VLINK="#660099">

<script src="/display.js"></script>

<FONT SIZE="3" FACE="HELVETICA">
<H1>
Web Remote Display
</H1>
<canvas id="display" width="480" height="320" style="border:1px solid #000000;">
Canvas not supported.
</canvas>
<TABLE width="100%"CELLSPACING=1 CELLPADDING=0 >
<TR>
<TD VALIGN=MIDDLE  ALIGN=left>
<P ALIGN=left id="status"></P>
</TD>
<TD VALIGN=MIDDLE  >
<DIV style="float: right;">
<BUTTON type="button" id="statusButton" style="height: 30px; width: 30px; border-radius: 50%; border: 1px;"/>
</DIV>
</TD>
</TR>
</TABLE>
</FONT>
</BODY>
</HTML>
)~";

#endif
