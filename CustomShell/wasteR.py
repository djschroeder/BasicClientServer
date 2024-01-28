import time

def expensive_computation():
    # Simulate a computationally expensive operation
    result = 0
    for _ in range(10**7):
        result += 1
    return result

i = 0
while i<100:
    i += 1
    expensive_computation()
    time.sleep(.1)  # Sleep for 1 second
