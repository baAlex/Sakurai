
Functions
---------

### TimerInit
- void

### TimerStop
- void

### TimerSleep
- `ax` Miliseconds to sleep
- Requires `ds` pointing to `seg_data`
- Call after timer Initialization

### TimerGet
- Returns in `ax` the miliseconds since initialization
- Requires `ds` pointing to `seg_data`
- Call after timer Initialization

### RenderInit
- void

### RenderStop
- void

### MemoryCopy
- `ds:di` Destination
- `es:si` Source
- `cx`    Size

### MemoryClean
- `ds:di` Destination
- `cx`    Size

### PrintLogString
- `ds:dx` String to print
- `cx`    Length

### PrintLogNumber
- `ax` Number to print

### PrintOut
- `ds:dx` Text to print ('$' terminated)

### Exit
- `al` Exit status
