data class ChallengeData(var game: String, var description: String, var roomID: Int, var condition: String, var resourceType: ResourceType,
                         var punishment : String, val minutes : Int, var partsWorked : BooleanArray, var calories : Int) {

    fun publishChallenge() = ServerConnect.sendToServer(Command.CreateChallenge.int, toString())

    override fun toString(): String {

        return "$resourceType$game$NIL$description$NIL$roomID$condition"
    }

    companion object {
        val resourceType = ResourceType.CHAL
        const val NIL = 0.toChar()
    }
}