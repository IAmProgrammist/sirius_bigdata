from fuzzywuzzy import fuzz
import time

univers=[]
extra_simbols=['(', ')', '"', '.', ',', '-', '«', '»']
def list_of_univers():
    with open('universities.txt', 'r', encoding='utf-8') as un:
        for line in un:
            univers.append(line)
def read():
    with open('queries.txt', 'r', encoding='utf-8') as quries:
        answer=open('answer.txt', 'w', encoding='utf-8')
        g=0

        for line in quries:
            best=''
            last_best_per=0
            g+=1
            checking= line[:-1]
            checking=checking.lower()

            for s in extra_simbols:
                if s in checking:
                    checking=checking.replace(s, '')

            for i in univers:
                univer=i.lower()

                for s in extra_simbols:
                    if s in univer:
                        univer = univer.replace(s, '')

                percent=fuzz.ratio(univer, checking)
                if percent>last_best_per:
                    print(g, checking, ':', univer)
                    last_best_per=percent
                    best=i
                elif percent==last_best_per:
                    print('!!!!!', g, best, '---', univer)
                    last_best_per = percent
                    best=i

            answer.write(best)

        answer.close()
if __name__ == '__main__':
    start_time = time.time()
    list_of_univers()
    read()
    print("--- %s seconds ---" % (time.time() - start_time))