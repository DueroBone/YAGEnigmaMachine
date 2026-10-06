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


n = int(argv[1])
# n = 1
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

sortedGrams = sorted(newGrams.items(), key=lambda x: x[0])
# print(sortedGrams)
print(str([item[1] for item in sortedGrams]).replace("[", "{").replace("]", "}"))
