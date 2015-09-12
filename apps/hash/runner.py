import subprocess


def main():
    results = []
    output = subprocess.check_output(['./hash'])
    for line in output.split('\n'):
        line = line.strip()
        if line:
            name, score = line.split(': ')
            score = float(score)
            results.append((name, score))

    results.sort(key=lambda p: p[1])
    for name, score in results:
        print('{}: {}'.format(name, score))


if __name__ == '__main__':
    main()
