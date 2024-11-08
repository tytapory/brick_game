package logic

import (
	"brick_game/common"
	"os"
	"testing"
)

func TestStart(t *testing.T) {
	os.Remove("max_score_race.txt")
	r := Race{}
	panicked := didPanic(func() { r.Start() })
	if panicked {
		t.Fatalf("panic on race start")
	}
	if r.highScore != 0 {
		t.Errorf("invalid highScore")
	}
	if r.enemyHeight != 5 {
		t.Errorf("invalid enemyHeight")
	}
	if r.wallDistance != 1 {
		t.Errorf("invalid wallDistance")
	}
	if r.playerCoords.X != 5 || r.playerCoords.Y != common.FieldH-5 {
		t.Errorf("invalid playerCoords")
	}
	if r.score != 0 {
		t.Errorf("invalid score")
	}
	if r.state != Pause {
		t.Errorf("invalid state")
	}
	if len(r.walls) == 0 {
		t.Errorf("walls matrix is empty")
	}
	if len(r.player) == 0 {
		t.Errorf("player matrix is empty")
	}
	if len(r.cars) == 0 {
		t.Errorf("cars matrix empty")
	}
}

func TestStartAction(t *testing.T) {
	r := Race{}
	r.Start()
	r.UserInput(common.Start)
	if r.state == Pause {
		t.Errorf("start button not working")
	}
}

func TestSpawnPlayer(t *testing.T) {
	r := Race{}
	r.Start()
	hitbox, _ := r.getPlayerHitbox(r.playerCoords)
	err := false
	for _, c := range hitbox {
		if !r.player[c.Y][c.X] {
			err = true
		}
	}
	if err {
		t.Errorf("hitbox does not exist")
	}
}

func TestUpdateCurrentFrame(t *testing.T) {
	r := Race{}
	r.Start()
	r.UserInput(common.Start)
	wallPixel := r.walls[0][0]
	panicked := didPanic(func() { r.UpdateCurrentState() })
	if panicked {
		t.Fatalf("panic in UpdateCurrentState")
	}
	if wallPixel == r.walls[0][0] {
		t.Errorf("walls not moving")
	}
}

func TestGenerateGameInfo(t *testing.T) {
	r := Race{}
	r.Start()
	panicked := didPanic(func() { r.GenerateGameInfo() })
	if panicked {
		t.Fatalf("panic in GenerateGameInfo")
	}
	gameInfo := r.GenerateGameInfo()
	if gameInfo.HighScore != r.highScore {
		t.Errorf("invalid highScore")
	}
	if gameInfo.Score != r.score {
		t.Errorf("invalid score")
	}
}

func TestMoveUp(t *testing.T) {
	r := Race{}
	r.Start()
	r.UserInput(common.Start)
	coords, _ := r.getPlayerHitbox(r.playerCoords)
	r.UserInput(common.Up)
	r.UpdateCurrentState()
	err := false
	for _, c := range coords {
		if !r.player[c.Y-1][c.X] {
			err = true
		}
	}
	if err {
		t.Errorf("car not moving on action Up")
	}
}

func TestMoveDown(t *testing.T) {
	r := Race{}
	r.Start()
	r.UserInput(common.Start)
	coords, _ := r.getPlayerHitbox(r.playerCoords)
	r.UserInput(common.Down)
	r.UpdateCurrentState()
	err := false
	for _, c := range coords {
		if !r.player[c.Y+1][c.X] {
			err = true
		}
	}
	if err {
		t.Errorf("car not moving on action Down")
	}
}

func TestMoveLeft(t *testing.T) {
	r := Race{}
	r.Start()
	r.UserInput(common.Start)
	coords, _ := r.getPlayerHitbox(r.playerCoords)
	r.UserInput(common.Left)
	r.UpdateCurrentState()
	err := false
	for _, c := range coords {
		if !r.player[c.Y][c.X-1] {
			err = true
		}
	}
	if err {
		t.Errorf("car not moving on action Left")
	}
}

func TestMoveRight(t *testing.T) {
	r := Race{}
	r.Start()
	r.UserInput(common.Start)
	coords, _ := r.getPlayerHitbox(r.playerCoords)
	r.UserInput(common.Right)
	r.UpdateCurrentState()
	err := false
	for _, c := range coords {
		if !r.player[c.Y][c.X+1] {
			err = true
		}
	}
	if err {
		t.Errorf("car not moving on action Right")
	}
}

func didPanic(f func()) bool {
	panicked := false
	defer func() {
		if r := recover(); r != nil {
			panicked = true
		}
	}()
	f()
	return panicked
}
