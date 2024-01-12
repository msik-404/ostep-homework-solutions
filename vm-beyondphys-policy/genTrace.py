#!/usr/bin/python

import random
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", help="Amount of pages", type=int, required=True)
    parser.add_argument("-l", help="Indicate whether cache locality occurs", action=argparse.BooleanOptionalAction)
    args = parser.parse_args()

    n = args.n

    output = []

    if not args.l:
        output = [random.randrange(n) for _ in range(n)]
    else:
        local_amount = int(n * 0.2)
        local_weights = 0.8 / local_amount
        other_weights = 0.2 / (n - local_amount)
        weight_func = lambda i: local_weights if i < local_amount else other_weights
        weights = [weight_func(i) for i in range(n)]
        output = random.choices(
                population= [i for i in range(n)],
                weights=weights,
                k=n
        )

    for i in range(n):
        if i != n-1:
            print(f"{output[i]},", end="")
        else:
            print(output[i])
