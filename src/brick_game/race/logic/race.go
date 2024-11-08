package logic

import (
	"brick_game/common"
	"io"
	"os"
	"strconv"
	"sync"

	"math/rand"
)

type Race struct {
	walls        [][]bool
	cars         [][]bool
	player       [][]bool
	score        int64
	highScore    int64
	state        State
	playerCoords coords
	enemyHeight  int64 //мы используем отдельную матрицу для машинок, и чтобы нам правильно их спавнить надо ее сделать больше на длину вражеской тачки, собственно этот оффсет тут и записан чтобы правильно смотреть столкновенияс
	currentFrame int64 // используем счетчик кадров для стабильного спавна машинок каждый n кадр
	wallDistance int64
}

type coords struct {
	X int64
	Y int64
}

type State int64

const (
	MovingUp State = iota
	MovingDown
	MovingRight
	MovingLeft
	Nothing
	Pause
	Dead
)

func (r *Race) Start() {
	r.highScore = getHighScore()
	r.enemyHeight = 5
	r.wallDistance = 1
	r.playerCoords.X = 5
	r.playerCoords.Y = common.FieldH - 5
	r.state = Pause
	r.walls = make([][]bool, common.FieldH)
	r.score = 0
	for i := range r.walls {
		r.walls[i] = make([]bool, common.FieldW)
	}
	r.player = make([][]bool, common.FieldH)
	for i := range r.player {
		r.player[i] = make([]bool, common.FieldW)
	}
	r.cars = make([][]bool, common.FieldH+r.enemyHeight)
	for i := range r.cars {
		r.cars[i] = make([]bool, common.FieldW)
	}
	r.spawnPlayer()
	r.generateWalls()
}

func (r *Race) spawnPlayer() {
	playerCar, _ := r.getPlayerHitbox(r.playerCoords)
	for i := range playerCar {
		r.player[playerCar[i].Y][playerCar[i].X] = true
	}
}

func (r *Race) UpdateCurrentState() {
	if r.state != Pause && r.state != Dead {
		if r.currentFrame%15 == 0 {
			r.spawnEnemy()
		}
		var wg sync.WaitGroup
		wg.Add(3)
		go func() {
			r.movePlayer()
			wg.Done()
		}()
		go func() {
			r.cars = scroll(r.cars, MovingDown)
			for i := range r.cars[0] {
				r.cars[0][i] = false
			}
			wg.Done()
		}()
		go func() {
			r.walls = scroll(r.walls, MovingDown)
			wg.Done()
		}()
		wg.Wait()
		dead, _ := r.isDead()
		if dead {
			r.state = Dead
		}
		r.currentFrame++
	}
}

func (r *Race) UserInput(action common.UserAction) {
	switch action {
	case common.Start:
		switch r.state {
		case Pause:
			r.state = Nothing
		case Dead:
			r.Start()
		}
	case common.Pause:
		if r.state != Dead {
			r.state = Pause
		}
	case common.Up:
		if r.state != Pause && r.state != Dead {
			r.state = MovingUp
			r.movePlayer()
		}
	case common.Down:
		if r.state != Pause && r.state != Dead {
			r.state = MovingDown
			r.movePlayer()
		}
	case common.Left:
		if r.state != Pause && r.state != Dead {
			r.state = MovingLeft
			r.movePlayer()
		}
	case common.Right:
		if r.state != Pause && r.state != Dead {
			r.state = MovingRight
			r.movePlayer()
		}
	}
}
func (r *Race) GenerateGameInfo() common.GameInfo {
	field := make([][]int64, common.FieldH)
	for i := range field {
		field[i] = make([]int64, common.FieldW)
		for j := range field[i] {
			val := 0
			if r.player[i][j] {
				val = 1
			}
			if r.walls[i][j] {
				val = 1
			}
			if r.cars[i+int(r.enemyHeight)][j] {
				val = 1
			}
			field[i][j] = int64(val)
		}
	}
	next := make([][]int64, 4)
	for i := range next {
		next[i] = make([]int64, 4)
	}

	level := r.score/400 + 1
	speed := 700 - level*140
	if speed < 50 {
		speed = 100
	}
	pause := 0
	if r.state == Pause {
		pause = 1
	}
	return common.GameInfo{
		Field:     field,
		Next:      next,
		Score:     r.score,
		HighScore: r.highScore,
		Level:     level,
		Speed:     speed,
		Pause:     int64(pause),
	}
}

func (r *Race) spawnEnemy() {
	model := getEnemyModel()
	leftCornerCoordinate := rand.Int63()%(common.FieldW-r.wallDistance-int64(len(model[0]))) + r.wallDistance

	for i := 0; i < 5; i++ {
		for j := leftCornerCoordinate; j < leftCornerCoordinate+3; j++ {
			r.cars[i][j] = model[i][j-leftCornerCoordinate]
		}
	}
	r.score += 100
	if r.highScore < r.score {
		r.highScore = r.score
		setHighScore(r.score)
	}
}

func (r *Race) generateWalls() {
	wall := r.getWall()
	for i := 0; i < len(r.walls); i++ {
		r.walls[i][r.wallDistance-1] = wall[i]
		r.walls[i][int64(len(r.walls[0]))-r.wallDistance] = wall[i]
	}
}

func (r *Race) getWall() []bool {
	result := make([]bool, len(r.player))
	for i := 0; i < int(len(r.player)); i++ {
		if i%2 == 0 {
			result[i] = false
		} else {
			result[i] = true
		}
	}
	return result
}

func getPlayerModel() [][]bool {
	return [][]bool{
		{false, true, false},
		{true, true, true},
		{false, true, false},
		{true, true, true},
	}
}

func getEnemyModel() [][]bool {
	return [][]bool{
		{false, true, false},
		{true, true, true},
		{false, true, false},
		{true, true, true},
		{false, true, false},
	}
}

func (r *Race) isDead() (bool, error) {
	playerCoords, err := r.getPlayerHitbox(coords{r.playerCoords.X, r.playerCoords.Y + r.enemyHeight})
	if err != nil {
		return false, err
	}
	for i := range playerCoords {
		if r.cars[playerCoords[i].Y][playerCoords[i].X] {
			return true, nil
		}
	}
	return false, nil
}

// getPlayerHitbox возвращает абсолютные координаты машинки в матрице врагов. Это нужно для того чтобы постоянно не сканить абсолютно все поле на столкновения а только клетки где находится игрок
func (r *Race) getPlayerHitbox(c coords) ([]coords, error) {
	return []coords{
		{c.X, c.Y - 1},
		{c.X - 1, c.Y},
		{c.X, c.Y},
		{c.X + 1, c.Y},
		{c.X, c.Y + 1},
		{c.X - 1, c.Y + 2},
		{c.X, c.Y + 2},
		{c.X + 1, c.Y + 2},
	}, nil
}

func (r *Race) movePlayer() {
	switch r.state {
	case MovingUp:
		if r.playerCoords.Y > 1 {
			r.player = scroll(r.player, MovingUp)
			r.playerCoords.Y--
		}
	case MovingDown:
		if r.playerCoords.Y < int64(len(r.player)-3) {
			r.player = scroll(r.player, MovingDown)
			r.playerCoords.Y++
		}
	case MovingLeft:
		if r.playerCoords.X > r.wallDistance+1 {
			r.player = scroll(r.player, MovingLeft)
			r.playerCoords.X--
		}
	case MovingRight:
		if r.playerCoords.X < int64(len(r.player[0]))-r.wallDistance-2 {
			r.player = scroll(r.player, MovingRight)
			r.playerCoords.X++
		}
	}
	r.state = Nothing
}

// scroll скроллит матрицу
func scroll(matrix [][]bool, state State) [][]bool {
	if len(matrix) == 0 || len(matrix[0]) == 0 {
		return matrix
	}

	var result [][]bool
	switch state {
	case MovingUp:
		result = make([][]bool, len(matrix))
		copy(result, matrix[1:])
		result[len(matrix)-1] = matrix[0]

	case MovingDown:
		result = make([][]bool, len(matrix))
		copy(result[1:], matrix[:len(matrix)-1])
		result[0] = matrix[len(matrix)-1]

	case MovingLeft:
		result = make([][]bool, len(matrix))
		var wg sync.WaitGroup
		for i := range matrix {
			wg.Add(1)
			go func() {
				result[i] = append(matrix[i][1:], matrix[i][0])
				wg.Done()
			}()
			wg.Wait()
		}

	case MovingRight:
		result = make([][]bool, len(matrix))
		var wg sync.WaitGroup
		for i := range matrix {
			wg.Add(1)
			go func() {
				result[i] = append([]bool{matrix[i][len(matrix[i])-1]}, matrix[i][:len(matrix[i])-1]...)
				wg.Done()
			}()
			wg.Wait()
		}
	}

	return result
}

func getHighScore() int64 {
	filename := "max_score_race.txt"
	createFileIfNotExist(filename)
	file, _ := os.Open(filename)
	defer file.Close()
	content, _ := io.ReadAll(file)
	res, _ := strconv.Atoi(string(content))
	return int64(res)
}

func setHighScore(s int64) {
	filename := "max_score_race.txt"
	createFileIfNotExist(filename)
	file, _ := os.OpenFile(filename, os.O_WRONLY|os.O_TRUNC, 0664)
	defer file.Close()
	numStr := strconv.Itoa(int(s))
	file.WriteString(numStr)
}

func createFileIfNotExist(filename string) {
	if _, err := os.Stat(filename); os.IsNotExist(err) {
		file, _ := os.Create(filename)
		file.Close()
		file2, _ := os.OpenFile(filename, os.O_WRONLY|os.O_TRUNC, 0664)
		defer file2.Close()
		file2.WriteString("0")
	}
}
