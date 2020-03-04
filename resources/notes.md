
Notes
=====

Functions
---------

### TimeInit (void)

### TimeStop (void)

### TimeSleep
- `ax` Milliseconds to sleep
- Requires `ds` pointing to `seg_data`
- Call after time Initialization

### TimeGet
- Returns in `ax` the milliseconds since initialization
- Requires `ds` pointing to `seg_data`
- Call after time Initialization

### InputInit (void)

### InputStop (void)

### InputClean
- Requires `ds` pointing to `seg_data`
- void

### RenderInit (void)

### RenderStop (void)

### MemoryCopy
- `es:di` Destination
- `ds:si` Source
- `cx`    Size

### MemoryClean
- `es:di` Destination
- `cx`    Size

### PrintLogString
- `ds:dx` String to print
- `cx`    Length

### PrintLogNumber
- `ax` Number to print

### PrintOut
- `ds:dx` Text to print ('$' terminated)

### FileOpen
- `ds:dx` Filename
- Returns handler in `ax`, `0x0000` in case of error

### FileClose
- `ax` File handler, `0x0000` is considered an invalid value, no operation is done and no error is throw

### FileRead
- `ax` File handler, `0x0000` is considered an invalid value, no operation is done and no error is throw
- `ds:dx` Destination
- `cx` Size

### Exit
- `al` Exit status
