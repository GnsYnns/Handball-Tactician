import * as jspb from 'google-protobuf'



export class SessionRequest extends jspb.Message {
  getClientId(): string;
  setClientId(value: string): SessionRequest;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): SessionRequest.AsObject;
  static toObject(includeInstance: boolean, msg: SessionRequest): SessionRequest.AsObject;
  static serializeBinaryToWriter(message: SessionRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): SessionRequest;
  static deserializeBinaryFromReader(message: SessionRequest, reader: jspb.BinaryReader): SessionRequest;
}

export namespace SessionRequest {
  export type AsObject = {
    clientId: string,
  }
}

export class FrameData extends jspb.Message {
  getTimestamp(): number;
  setTimestamp(value: number): FrameData;

  getPlayersList(): Array<Player>;
  setPlayersList(value: Array<Player>): FrameData;
  clearPlayersList(): FrameData;
  addPlayers(value?: Player, index?: number): Player;

  getBall(): Ball | undefined;
  setBall(value?: Ball): FrameData;
  hasBall(): boolean;
  clearBall(): FrameData;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): FrameData.AsObject;
  static toObject(includeInstance: boolean, msg: FrameData): FrameData.AsObject;
  static serializeBinaryToWriter(message: FrameData, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): FrameData;
  static deserializeBinaryFromReader(message: FrameData, reader: jspb.BinaryReader): FrameData;
}

export namespace FrameData {
  export type AsObject = {
    timestamp: number,
    playersList: Array<Player.AsObject>,
    ball?: Ball.AsObject,
  }
}

export class Player extends jspb.Message {
  getId(): number;
  setId(value: number): Player;

  getX(): number;
  setX(value: number): Player;

  getY(): number;
  setY(value: number): Player;

  getTeam(): string;
  setTeam(value: string): Player;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): Player.AsObject;
  static toObject(includeInstance: boolean, msg: Player): Player.AsObject;
  static serializeBinaryToWriter(message: Player, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): Player;
  static deserializeBinaryFromReader(message: Player, reader: jspb.BinaryReader): Player;
}

export namespace Player {
  export type AsObject = {
    id: number,
    x: number,
    y: number,
    team: string,
  }
}

export class Ball extends jspb.Message {
  getX(): number;
  setX(value: number): Ball;

  getY(): number;
  setY(value: number): Ball;

  getSpeed(): number;
  setSpeed(value: number): Ball;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): Ball.AsObject;
  static toObject(includeInstance: boolean, msg: Ball): Ball.AsObject;
  static serializeBinaryToWriter(message: Ball, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): Ball;
  static deserializeBinaryFromReader(message: Ball, reader: jspb.BinaryReader): Ball;
}

export namespace Ball {
  export type AsObject = {
    x: number,
    y: number,
    speed: number,
  }
}

