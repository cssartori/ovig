Original data packaged around 2018-11-24 by OpenAddresses (http://openaddresses.io).

Website: http://fbinter.stadt-berlin.de/fb/berlin/service.jsp?id=a_hauskoordinaten@senstadt&type=FEED
License: Nutzungsbestimmungen Geoportal Berlin (http://www.stadtentwicklung.berlin.de/geoinformation/download/nutzIII.pdf)

--------------------------------------------------------------------------------

Data version information:
file:   de/berlin
date:   2018-12-03
output: 1f4655d
code:   6.8.10
source: b3714e7

NOTE: The data available in the file 'berlin.txt' has been extracted from the
original data packaged by OpenAddresses. The extraction was performed as 
follows: 
    1) All addresses in the original 'de/berlin.csv' file accompanying the 
    README were read;
    2) From all addresses, those in which the field city (6th column) had the
    value "Berlin" were kept;
    3) Then, the geo location (lat,lon) from each Berlin address was kept, the
    remaining information was discarded;
    4) The geo locations were saved to file 'berlin.txt' in a format to be read
    by instance generator provided within this repository.
    
The full process can be performed using the python script 'ext.py' available
within this same folder.

--------------------------------------------------------------------------------
