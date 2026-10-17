from private import textFile
import re
from sys import argv

text = textFile.read().strip().replace("\n", " ").replace(" ", "").upper()
text = re.sub(r"\d+ ", "", text)


def generate_ngrams(text, n):
    ngrams = {}
    for i in range(len(text) - n + 1):
        ngram = text[i : i + n]
        if ngram in ngrams:
            ngrams[ngram] += 1
        else:
            ngrams[ngram] = 1
    return ngrams


def c(letter):
    return ord(letter) - ord("A")


n = int(argv[1]) if len(argv) > 1 else 1
grams = generate_ngrams(text, n)
newGrams = {}
for key in grams.keys():
    newKey = 0
    for i in range(len(key)):
        newKey += c(key[i]) * (26 ** (len(key) - i - 1))
    newGrams[newKey] = grams[key]

for i in range(26**n):
    if i not in newGrams.keys():
        newGrams[i] = 0

length = len(text)


def nested_values(values, dimensions):
    if dimensions == 1:
        return values

    size = 26 ** (dimensions - 1)
    return [
        nested_values(values[i : i + size], dimensions - 1)
        for i in range(0, len(values), size)
    ]


def c_initializer(values):
    if isinstance(values[0], list):
        return "{" + ", ".join(c_initializer(value) for value in values) + "}"
    return "{" + ", ".join(str(value / length) for value in values) + "}"


values = [newGrams[i] for i in range(26**n)]
print(c_initializer(nested_values(values, n)))
