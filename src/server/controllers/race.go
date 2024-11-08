package server

import (
	race "brick_game/brick_game/race/logic"
	"brick_game/common"
	"brick_game/common/web_controller"
)

type RaceDecorator struct {
	r *race.Race
}

func (r RaceDecorator) UpdateCurrentState() common.GameInfo {
	r.r.UpdateCurrentState()
	return r.r.GenerateGameInfo()
}

func (r RaceDecorator) UserInput(action common.UserAction, hold bool) {
	r.r.UserInput(action)
}

func RunRace() web_controller.Channels {
	raceInstance := &race.Race{}
	raceInstance.Start()

	r := RaceDecorator{r: raceInstance}

	return web_controller.Run(r)
}
