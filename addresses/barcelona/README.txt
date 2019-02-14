Original data packaged around 2018-11-24 by OpenAddresses (http://openaddresses.io).

Website: http://blog-idee.blogspot.com.es/2014/03/actualizacion-de-los-datos-de-descarga.html
License: http://www.catastro.minhap.es/ayuda/legislacion/ovc/resoluciondgc20110323_tfs.pdf

--------------------------------------------------------------------------------

Data version information:
file:   es/25831
date:   2018-12-03
output: 29a07fe
code:   6.8.10
source: 2936aa2

NOTE: The data available in the file 'barcelona.txt' has been extracted from the
original data packaged by OpenAddresses. The extraction was performed as 
follows: 
    1) All addresses in the original 'es/25831.csv' file accompanying 
    the README were read;
    2) From all addresses, those in which the geo location (lat,lon) was within 
    the approximated square where Barcelona city is located were kept;
    3) Then, the geo location (lat,lon) from each Barcelona address was kept,
    the remaining information was discarded;
    4) The geo locations were saved to file 'barcelona.txt' in a format to be 
    read by instance generator provided within this repository.
    
The full process can be performed using the python script 'ext.py' available
within this same folder.

--------------------------------------------------------------------------------
