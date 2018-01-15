ffen2tex %1.tex out.tmp
msr spechars.msr out.tmp out.tex
latex out.tex
dvips out.dvi
ps2pdf out.ps
del %1.pdf
rename out.pdf %1.pdf
del out.*
