package web_controller

import (
	"brick_game/common"
	"fmt"
	"time"
)

type ActionHold struct {
	Id   common.UserAction
	Hold bool
}

type Channels struct {
	Input  chan ActionHold
	Output chan common.GameInfo
	Done   chan struct{}
}

type WebController interface {
	UpdateCurrentState() common.GameInfo
	UserInput(common.UserAction, bool)
}

func Run(c WebController) Channels {
	channels := Channels{make(chan ActionHold, 3), make(chan common.GameInfo, 1), make(chan struct{})}
	go mainLoop(c, channels)
	return channels
}

func mainLoop(c WebController, channels Channels) {
	defer close(channels.Input)
	defer close(channels.Output)
	defer close(channels.Done)
	noActionFrames := 0
	var action ActionHold
	action.Id = common.Nothing
	action.Hold = false
	for action.Id != common.Terminate && noActionFrames < 150 {
		c.UserInput(action.Id, action.Hold)
		action.Id = common.Nothing
		action.Hold = false
		select {
		case <-channels.Output:
		default:
		}
		info := c.UpdateCurrentState()
		channels.Output <- info
		actionFlag := false
		done := false
		go func(done *bool) {
			for !*done {
				select {
				case input := <-channels.Input:
					action = input
					actionFlag = true
				default:
				}
			}
		}(&done)
		duration := time.Duration(info.Speed)
		time.Sleep(time.Millisecond * duration)
		fmt.Printf("speed: %d. actionid %d.\n", info.Speed, action.Id)
		done = true
		if !actionFlag {
			noActionFrames++
		} else {
			noActionFrames = 0
		}
	}
}
