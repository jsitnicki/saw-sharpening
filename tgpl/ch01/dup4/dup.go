package main

import (
	"bufio"
	"fmt"
	"os"
)

func main() {
	fileCounts := make(map[string]map[string]int)
	files := os.Args[1:]
	if len(files) == 0 {
                fileCounts["(standard input)"] = make(map[string]int)
                counts := fileCounts["(standard input)"]
		countLines(os.Stdin, counts)
	} else {
		for _, arg := range files {
			f, err := os.Open(arg)
			if err != nil {
				fmt.Fprintf(os.Stderr, "dup4: %v\n", err)
				continue
			}
                        fileCounts[arg] = make(map[string]int)
                        counts := fileCounts[arg]
			countLines(f, counts)
			f.Close()
		}
	}

        for file, counts := range fileCounts {
                for line, n := range counts {
                        if n > 1 {
                                fmt.Printf("%s:\t%d\t%s\n", file, n, line)
                        }
                }
        }
}

func countLines(f *os.File, counts map[string]int) {
	input := bufio.NewScanner(f)
	for input.Scan() {
		counts[input.Text()]++
	}
}
