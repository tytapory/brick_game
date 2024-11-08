package server

/*
#include "../../brick_game/tetris/logic/tetris.h"
#include "../../brick_game/tetris/logic/tetris.c"
*/
import "C"
import (
	"brick_game/common"
	"brick_game/common/web_controller"
	"fmt"
	"time"
	"unsafe"
)

type TetrisDecorator struct {
	t *C.State_t
}

func (t TetrisDecorator) UpdateCurrentState() common.GameInfo {
	info := GameInfoCToGo(C.updateCurrentStateFrom(t.t))
	info.Speed = 500 - (info.Level * 40)
	return info
}

func (t TetrisDecorator) UserInput(action common.UserAction, hold bool) {
	state := t.t
	if state.pause != C.None && state.status != C.Initial {
		if action == common.Pause {
			C.userInputTo(C.UserAction_t(common.Start), t.t)
		} else {
			return
		}
	}
	fmt.Println(state.status)
	if state.status == C.Initial {
		updateUserAction(action, t)
	} else if state.status == C.Spawn {
		C.spawn_new_block_from(t.t)
	} else if state.status == C.Moving {
		updateUserAction(action, t)
	} else if state.status == C.Shifting {
		C.shift_block_from(t.t)
	} else if state.status == C.Attaching {
		C.attach_block_from(t.t)
	} else if state.status == C.GameOver {
		C.finish_game_from(t.t)
	}
}

func updateUserAction(action common.UserAction, t TetrisDecorator) {
	//time_left := C.process_timer_from(state)
	if action == common.Down && t.t.status == C.Moving {
		for C.block_is_attached_from(t.t) == 0 {
			t.t.coord_x++
			C.updateCurrentStateFrom(t.t)
		}
		C.userInputTo(C.UserAction_t(action), t.t)
	} else if action == common.Nothing && t.t.status != C.Initial {
		t.t.status = C.Shifting
	} else {
		C.userInputTo(C.UserAction_t(action), t.t)
	}
}

func RunTetris() web_controller.Channels {
	tetris := (*C.State_t)(C.malloc(C.sizeof_State_t))
	C.initialize_state_from(tetris)
	t := TetrisDecorator{t: tetris}
	channels := web_controller.Run(t)
	go func(chans web_controller.Channels, tetris *C.State_t) {
		stop := false
		for !stop {
			select {
			case <-chans.Done:
				C.free(unsafe.Pointer(tetris))
				stop = true
			default:
			}
		}
		time.Sleep(time.Millisecond * 1000)
	}(channels, t.t)
	return channels
}
