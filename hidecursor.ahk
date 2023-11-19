detectHiddenWindows, On
gui +hwndgHwnd
gui,show,hide w1 h1
winset,transparent,1,ahk_id %gHwnd%
gui +alwaysOnTop +toolWindow -caption +0x80000000
return

tog:=0

[::
if (!tog)
{
    dllcall("ShowCursor","uint",0)
    mousegetpos,mx,my,active
    gui +Owner%active%
    gui,show,x%mx% y%my% noactivate
    tog:=1
}
return

]::
if (tog)
{
    gui,cancel
    dllcall("ShowCursor","uint",1)
    tog:=0
}
return