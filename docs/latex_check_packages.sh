##
# shell script which checks whether required packages
# are available for building the pdf documentation
#

declare -a LATEX_PACKAGES=(
"amsfonts
amssymb
amsbsy
amsgen
amsmath
amsopn
amstext
babel
alltt
fontenc
ifthen
inputenc
makeidx
textcomp
newfloat
capt-of
cmap
environ
eqparbox
fancybox
fancyhdr
fancyvrb
float
framed
color
graphics
graphicx
keyval
trig
hyperref
nameref
iftex
ifxetex
footnote
multirow
needspace
auxhook
epstopdf-base
gettitlestring
grfext
hobsub-generic
hobsub-hyperref
hypcap
infwarerr
kvoptions
ltxcmds
rerunfilecheck
parskip
times
tabulary
threeparttable
titlesec
array
longtable
trimspaces
upquote
url
wrapfig"
)

# array holding names of missing packages
declare -a MISSING=()

printf "\n%-22s Installed\n" "Package"
printf "================================\n"
# iterate required packages
for i in $(echo "$LATEX_PACKAGES");
do
  f="$i.sty"
  printf "%-22s " $f;
  FOUND="Yes"
  # adds package to MISSING packages if the can't be found
  # by neither kpsewhich nor find
  if [ $(kpsewhich $i.sty | wc -l ) -eq 0 ] && [ $(find ./ -name $f | wc -l) -eq 0 ]; then
    FOUND="No"
    MISSING+=($i)
  fi
  echo $FOUND
done;

printf "\n\n"

FAILED=${#MISSING[@]}
if [[ $FAILED > 0 ]]; then
  # Print list of missing packages if any
  echo "Found $FAILED missing packages."
  echo ${MISSING[@]} | sed 's/ /\n/g'
  echo "It is recommended to install 'texlive-latex-extra' to get the missing packages"
else
  echo "Found no missing packages"
fi

