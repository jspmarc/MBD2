#!/bin/python3
import string
import random

if __name__ == '__main__':
    print('INSERT INTO orang(nama) VALUES')
    count = 10_000_000
    for i in range(0, count):
        print("('", end='')
        name = ''.join(
            random.choices(
                string.ascii_letters + string.digits, k=8
            )
        )
        print(name, end='')
        print("')" + (',' if i != count - 1 else ';'))
