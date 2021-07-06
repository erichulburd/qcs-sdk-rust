/* Warning, this file is autogenerated by cbindgen. Don't modify this manually. */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>


/**
 * The available result codes from running [`list_quantum_processors`]
 */
enum ListQuantumProcessorsResult {
    ListQuantumProcessorsResult_Success = 0,
    ListQuantumProcessorsResult_CouldNotQueryQCS = 1,
    ListQuantumProcessorsResult_Unauthorized = 2,
};
typedef uint8_t ListQuantumProcessorsResult;

/**
 * Codes indicating the possible results of calling [`run_program_on_qvm`]. Every [`QVMResponse`]
 * will have one of these statuses in their `status_code` field. Note that in the generated C
 * headers, each variant will be prefixed with `QVMStatus` to prevent naming conflicts
 * (e.g. `QVMStatus_Success`).
 */
enum QVMStatus {
    /**
     * Program was run successfully, the [`QVMResponse`] containing this has valid data in other fields.
     */
    QVMStatus_Success = 0,
    /**
     * The Program provided was not valid UTF-8 and could not be decoded for processing.
     */
    QVMStatus_ProgramIsNotUtf8 = 1,
    /**
     * Something prevented this library from attempting to make the request, if this happens
     * it's probably a bug.
     */
    QVMStatus_CannotMakeRequest = 2,
    /**
     * QVM did not respond with a results in the specified register.
     */
    QVMStatus_NoResults = 3,
    /**
     * One or more shots had differing numbers of result registers, this could be a bug with QVM.
     */
    QVMStatus_InconsistentShotLength = 5,
    /**
     * A request to QVM was attempted but failed, is it running?
     */
    QVMStatus_UnableToCommunicateWithQVM = 6,
    /**
     * The provided `register_name` was not valid UTF-8
     */
    QVMStatus_RegisterIsNotUtf8 = 7,
};
typedef uint8_t QVMStatus;

/**
 * Represents the information of a single available processor
 */
typedef struct QuantumProcessor {
    /**
     * Unique identifier for a Processor.
     */
    char *id;
} QuantumProcessor;

/**
 * The response from [`list_quantum_processors`], contains an array of strings.
 */
typedef struct ListQuantumProcessorResponse {
    /**
     * The result code of the function call. Anything other than [`ListQuantumProcessorsResult::Success`]
     * will result in a null `processors`.
     */
    ListQuantumProcessorsResult result;
    /**
     * Array of all available processors. This will be NULL if `result` is not Success
     */
    struct QuantumProcessor *processors;
    /**
     * The length of the array to use for iterating.
     */
    size_t len;
    /**
     * The total capacity of the array in case you'd like to modify it.
     */
    size_t cap;
} ListQuantumProcessorResponse;

/**
 * The return value of [`run_program_on_qvm`].
 *
 * # Safety
 * In order to properly free the memory allocated in this struct, call [`free_qvm_response`]
 * with any instances created.
 *
 * # Example
 * If you have a Quil program with an "ro" register containing two items:
 *
 * ```quil
 * DECLARE ro BIT[2]
 * ```
 * and you run that program 3 times (shots)
 *
 * ```C
 * QVMResponse response = run_program_on_qvm(program, 3, "ro");
 * ```
 * If `status_code` is `Success` then `results_by_shot` will look something like:
 *
 * ```
 * results_by_shot = [[0, 0], [0, 0], [0, 0]]
 * ```
 *
 * where `results_by_shot[shot][bit]` can access the value of `ro[bit]` for a given `shot`.
 */
typedef struct QVMResponse {
    /**
     * A 2-D array of integers containing the measurements into register provided as
     * `register_name`. There will be one value per declared space in the register per "shot"
     * (run of the program).
     */
    unsigned char **results_by_shot;
    /**
     * The number of times the program ran (should be the same as the `num_shots` param to
     * [`run_program_on_qvm`]. This is the outer dimension of `results_by_shot`.
     */
    unsigned int number_of_shots;
    /**
     * How many bits were measured in the program in one shot. This is the inner dimension of
     * `results_by_shot`.
     */
    unsigned int shot_length;
    /**
     * Tells you whether or not the request to the QVM was successful. If the status
     * code is [`QVMStatus::Success`], then `results_by_shot` will be populated.
     * If not, `results_by_shot` will be `NULL`.
     */
    QVMStatus status_code;
} QVMResponse;

/**
 * This function exists to deallocate the memory that was allocated by a call to [`list_quantum_processors`]
 *
 * # Safety
 *
 * The `response` passed in here must be a valid [`ListQuantumProcessorResponse`] as created by
 * [`list_quantum_processors`].
 */
void free_quantum_processors(struct ListQuantumProcessorResponse response);

/**
 * Frees the memory of a [`QVMResponse`] as allocated by [`run_program_on_qvm`]
 *
 * # Safety
 * This function should only be called with the result of [`run_program_on_qvm`]
 */
void free_qvm_response(struct QVMResponse response);

/**
 * Return a comma-separated list of available quantum processors
 *
 * # Safety
 *
 * In order to safely operate this function:
 *
 * 1. The return value of this function __must__ be passed into [`free_quantum_processors`] in
 *     order to deallocate the memory.
 *
 */
struct ListQuantumProcessorResponse list_quantum_processors(void);

/**
 * Given a Quil program as a string, run that program on a local QVM.
 *
 * # Safety
 *
 * In order to run this function safely, you must provide the return value from this
 * function to [`free_qvm_response`] once you're done with it. The inputs `program` and
 * `register_name` must be valid, nul-terminated, non-null strings which remain constant for
 * the duration of this function.
 *
 * # Usage
 *
 * In order to execute, QVM must be running at <http://localhost:5000>.
 *
 * # Arguments
 *
 * 1. `program`: A string containing a valid Quil program. Any measurements that you'd like
 * to get back out must be in a register matching `register_name`. For example, if you have
 * `MEASURE 0 ro[0]` then `register_name` should be `"ro"`.
 * 2. `num_shots` is the number of times you'd like to run the program.
 * 3. `register_name`:
 *
 * # Errors
 * This program will return a [`QVMResponse`] with a `status_code` corresponding to any errors that
 * occur. See [`QVMStatus`] for more details on possible errors.
 *
 * # Example
 *
 * ```c
 * #include <stdio.h>
 * #include "../libqcs.h"
 *
 * char* BELL_STATE_PROGRAM =
 *         "DECLARE ro BIT[2]\n"
 *         "H 0\n"
 *         "CNOT 0 1\n"
 *         "MEASURE 0 ro[0]\n"
 *         "MEASURE 1 ro[1]\n";
 *
 * int main() {
 *     uint8_t shots = 10;
 *     QVMResponse response = run_program_on_qvm(BELL_STATE_PROGRAM, shots, "ro");
 *
 *     if (response.status_code != QVMStatus_Success) {
 *         // Something went wrong running the program
 *         return 1;
 *     }
 *
 *     for (int shot = 0; shot < response.number_of_shots; shot++) {
 *         int bit_0 = response.results_by_shot[shot][0];
 *         int bit_1 = response.results_by_shot[shot][1];
 *         // With this program, bit_0 should always equal bit_1
 *     }
 *
 *     free_qvm_response(response);
 *
 *     return 0;
 * }
 * ```
 */
struct QVMResponse run_program_on_qvm(char *program, unsigned int num_shots, char *register_name);
