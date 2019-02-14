Original data packaged around 2018-11-24 by OpenAddresses (http://openaddresses.io).

Website: http://www.censo2010.ibge.gov.br/cnefe/
License: Unknown

--------------------------------------------------------------------------------

Data version information:
file:   br/rs/statewide
date:   2018-12-03
output: 1d2bfe3
code:   6.8.10
source: bcd3028

NOTE: The data available in the file 'poa.txt' has been extracted from the
original data packaged by OpenAddresses. The extraction was performed as 
follows: 
    1) All addresses in the original 'br/rs/statewide.csv' file accompanying 
    the README were read;
    2) From all addresses, those in which the field city (6th column) had the
    value "Porto Alegre" were kept;
    3) Then, the geo location (lat,lon) from each Porto Alegre address was kept,
    the remaining information was discarded;
    4) The geo locations were saved to file 'poa.txt' in a format to be read
    by instance generator provided within this repository.
    
The full process can be performed using the python script 'ext.py' available
within this same folder.

--------------------------------------------------------------------------------
