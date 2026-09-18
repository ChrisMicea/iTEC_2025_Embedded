import serial
import time

# Configuration
SERIAL_PORT = 'COM12'  # Change to your port
BAUD_RATE = 9600       # Must match HC-05 baud rate
TIMEOUT = 6.0          # Increased timeout for matrix transfer
MATRIX_ROWS = 100       # Expected rows in matrix
MATRIX_COLS = 100       # Expected columns in matrix

def main():
    # Initialize matrix
    matrix = [[0 for j in range(MATRIX_COLS)] for i in range(MATRIX_ROWS)]
    current_row = 0
    
    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT) as ser:
            print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud")
            
            # 1. Send 'N' to request matrix
            print("Sending 'N' to stop robot...")
            ser.write(b'N')
            ser.flush()  # Ensure data is sent immediately
            
            # 2. Wait for response (adjust based on your device's response time)
            time.sleep(0.5)
            
            # 3. Read matrix line by line
            print("Reading matrix...")
            while current_row < MATRIX_ROWS:
                try:
                    # Read until newline
                    line = ser.readline()

                    try:
                        decoded = line.decode('utf-8', errors='ignore').strip()
                    except UnicodeDecodeError:
                        print("Warning: Non-decodable line")
                        continue

                    if len(decoded) < MATRIX_COLS:
                        print(f"Incomplete line at row {current_row}: '{decoded}' ({len(decoded)} chars)")
                        current_row += 1
                        continue

                    matrix[current_row] = list(line[:MATRIX_COLS])  # Truncate to expected columns
                    #print(f"Row {current_row}: {matrix[current_row]}")
                    print(f'Row {current_row}')
                    current_row += 1
                        
                except UnicodeDecodeError:
                    print("Warning: Received non-UTF-8 data")
                except KeyboardInterrupt:
                    print("\nStopping...")
                    break
                except Exception as e:
                    print(f"Error reading data: {e}")
                    break
            
            # Print complete matrix
            print("\nComplete Matrix:")
            for row in matrix:
                print(''.join(row) if isinstance(row[0], str) else row)
                
    except serial.SerialException as e:
        print(f"Failed to open {SERIAL_PORT}: {e}")
    except KeyboardInterrupt:
        print("\nStopped by user")

if __name__ == "__main__":
    main()