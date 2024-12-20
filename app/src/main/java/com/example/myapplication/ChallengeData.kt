data class ChallengeData(var game: String, var description: String, var roomID: Int, var condition: String, var resourceType: ResourceType) {

    fun publishChallenge() = ServerConnect.sendToServer(Command.CreateChallenge.int, toString())

    override fun toString(): String {

        return "$resourceType$game$nil$description$nil$roomID$condition"
    }
    companion object {
        val resourceType = ResourceType.CHAL
        val nil = 0.toChar()
    }
}