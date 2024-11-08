package main

/*
#cgo CFLAGS: -I../../../../gui/cli
#cgo LDFLAGS: -lncurses
#include "controller.h"
#include "controller.c"
#include "../../../../gui/cli/console_front.c"
*/
import "C"
import (
	race "brick_game/brick_game/race/logic"
	"brick_game/common"
	"brick_game/common/go_controller"
	"errors"
	"unsafe"
)

var Race race.Race

type RaceModel struct{}
type CliView struct{}
type CliController struct{}

func (m RaceModel) UpdateCurrentState() {
	Race.UpdateCurrentState()
}

func (m RaceModel) GetGameInfo() common.GameInfo {
	return Race.GenerateGameInfo()
}

func (m RaceModel) UserInput(action common.UserAction) {
	Race.UserInput(action)
}

func ArrayToPtr(arr [][]int64) **C.int {
	result := C.create_matrix(C.FIELD_H, C.FIELD_W)
	for i := int64(0); i < common.FieldH; i++ {
		if i >= int64(len(arr)) { // Проверка границ массива
			break
		}
		rowPtr := *(**C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(result)) + uintptr(i)*unsafe.Sizeof(uintptr(0))))
		for j := int64(0); j < common.FieldW; j++ {
			if j >= int64(len(arr[i])) { // Проверка границ массива
				break
			}
			elementPtr := (*C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(rowPtr)) + uintptr(j)*unsafe.Sizeof(C.int(0))))
			*elementPtr = C.int(arr[i][j])
		}
	}
	return result
}

func (v CliView) Render(current common.GameInfo) {
	target := C.GameInfo_t{
		field:      ArrayToPtr(current.Field),
		next:       ArrayToPtr(current.Next),
		score:      C.int(current.Score),
		high_score: C.int(current.HighScore),
		level:      C.int(current.Level),
		speed:      C.int(current.Speed),
		pause:      C.int(current.Pause),
	}
	C.renderScr(target)
}

func (c CliController) Init() {
	C.initialize()
}

func (c CliController) GetInput() (common.UserAction, error) {
	input := C.getInput()
	var action common.UserAction
	switch input {
	case C.int(C.KEY_UP):
		action = common.Up
	case C.int(C.KEY_DOWN):
		action = common.Down
	case C.int(C.KEY_LEFT):
		action = common.Left
	case C.int(C.KEY_RIGHT):
		action = common.Right
	case C.int(C.TERMINATE_BUTTON):
		return action, errors.New("terminate")
	case C.int(C.PAUSE_BUTTON):
		action = common.Pause
	case C.int(C.START_BUTTON):
		action = common.Start
	default:
		action = common.Nothing
	}
	return action, nil
}

func main() {
	raceModel := &RaceModel{}
	cliView := CliView{}
	cliController := CliController{}
	Race.Start()
	go_controller.Start(raceModel, cliView, cliController)
}
