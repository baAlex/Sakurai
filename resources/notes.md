
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

### PrintLog
- `ds:dx` Text to print
- `cx`    Length

### PrintOut
- `ds:dx` Text to print ('$' terminated)

### Exit
- `al` Exit status
